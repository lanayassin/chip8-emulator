/**
 * @file processor.c
 * @brief Implémentation du processeur Chip-8.
 * 
 * Ce fichier contient l'implémentation du cycle d'exécution du processeur, la gestion des opcodes, des timers, du clavier et du speaker.
 */
#include "processor.h"
#include "memory.h"
#include "opcode.h"
#include "display/sprite.h"
#include "display/display.h"
#include "keyboard/keyboard.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "speaker/speaker.h"

/**
 * @brief Initialise le processeur Chip-8.
 * 
 * Initialise les registres, la pile et les timers.
 * Associe les différents périphériques.
 * 
 * @param cpu Pointeur vers la structure processeur
 * @param ram Mémoire principale
 * @param display Module d'affichage
 * @param Keyboard Module clavier
 * @param Speaker Module speaker
 * @return 0 en cas de succés, -1 en cas d'erreur
 */
int processor_init(struct processor *cpu, struct memory* ram, struct Display* display, struct Keyboard* Keyboard, struct Speaker* Speaker) {
    if (!cpu || !ram) return -1;
    
    //Initialisation du générateur de nombres aléatoires
    srand((unsigned)time(NULL));

    //Réinitialisation des registres et du registre I
    memset(cpu->V, 0, sizeof(cpu->V));
    cpu->I = 0;
    cpu->PC = START_ADDRESS;

    //Réinitialisation des timers
    cpu->DT = 0;
    cpu->ST = 0;

    //Réinitialisation de la pile
    cpu->SP = 0;
    memset(cpu->stack, 0, sizeof(cpu->stack));

    //Assignation des périphériques
    cpu->RAM = ram;
    cpu->Display = display;
    cpu->Keyboard = Keyboard;
    cpu->Speaker = Speaker;

    cpu->dt_last_update = SDL_GetTicks();

    //Initialisation de la gestion du clavier
    cpu->waiting_reg    = 0xFF;
    cpu->key_wait_phase = KEY_WAIT_NONE;
    cpu->latched_key    = -1;

    //Flags de contrôle
    cpu->draw_allowed = 1;
    cpu->stalled = 0;

    memset(cpu->key_prev, 0, sizeof(cpu->key_prev));

    return 0;
}

/**
 * @brief Détruit le processeur.
 * 
 * Met le pointeur du processeur à NULL.
 * 
 * @param cpu Double pointeur vers la structure processeur
 */
void processor_destroy(struct processor **cpu) {
    if (cpu) {
        *cpu = NULL;
    }
}

/* Vérifie que le PC permet la lecture de deux octets */
static inline int check_pc2(struct processor *cpu) {
    return (cpu->PC +1 <MEMORY_SIZE)?0 : 1;
}

/**
 * @brief Récupère l'instruction suivante en mémoire.
 * 
 * Lit deux octets à l'adresse PC, puis incrémente PC.
 * 
 * @param cpu Pointeur vers le processeur.
 * @return Opcode 16 bits, ou -1 en cas d'erreur.
 */
uint16_t processor_fetch(struct processor *cpu) {
    assert(cpu && cpu->RAM);

    if (check_pc2(cpu) != 0) {
        fprintf(stderr, "PC out of bounds before fetch: PC=0x%03X\n", cpu->PC);
        return 0;
    }

    uint16_t opcode = memory_read_instruction(cpu->RAM, cpu->PC);
    cpu->PC += 2; 
    return opcode;
}


/* Capture l'état actuel du clavier*/
static inline void keyboard_snapshot(struct processor *cpu, uint8_t curr[16]) {
    for (uint8_t k = 0; k < 16; ++k) {
        int state = KEY_UP;
        if (cpu->Keyboard && Keyboard_get(cpu->Keyboard, k, &state) == 0) {
            curr[k] = (state == KEY_DOWN) ? 1 : 0;
        } else {
            curr[k] = 0;
        }
    }
}

/**
 * @brief Exécute un cycle du processeur.
 * 
 * Gère l'attente d'entrée clavier, la récupération de l'opcode et son exécution via dispatch.
 * 
 * @param cpu Pointeur vers le processeur.
 */
void processor_step(struct processor *cpu) {
    assert(cpu && cpu->RAM);

    uint8_t curr[16];
    keyboard_snapshot(cpu, curr);

    //Gestion de l'attente d'une touche pour Fx0A
    if (cpu->key_wait_phase != KEY_WAIT_NONE) {
        if (cpu->key_wait_phase == KEY_WAIT_PRESS) {
            for (uint8_t k = 0; k < 16; ++k) {
                if (curr[k] && !cpu->key_prev[k]) {
                    cpu->V[cpu->waiting_reg] = k;  
                    cpu->latched_key = (int8_t)k;
                    cpu->key_wait_phase = KEY_WAIT_RELEASE;
                    break;
                }
            }
            if (cpu->key_wait_phase == KEY_WAIT_PRESS) {
                memcpy(cpu->key_prev, curr, sizeof(curr));
                return; 
            }
        }

        if (cpu->key_wait_phase == KEY_WAIT_RELEASE) {
            int lk = cpu->latched_key;
            if (lk >= 0) {
                //Attente du relâchement de la touche
                if (!curr[lk] && cpu->key_prev[lk]) {
                    cpu->key_wait_phase = KEY_WAIT_NONE; 
                } else {
                    memcpy(cpu->key_prev, curr, sizeof(curr));
                    return; 
                }
            } else {

                cpu->key_wait_phase = KEY_WAIT_PRESS;
                memcpy(cpu->key_prev, curr, sizeof(curr));
                return;
            }
        }
    }

    //Si le processeur est en pause, ne rien faire
    if (cpu->stalled) {
        return;  
    }

    //Récupération et exécution de l'opcode
    uint16_t opcode = processor_fetch(cpu);
    uint8_t op = (uint8_t)((opcode & 0xF000) >> 12);

    opcode_handler handler = dispatch[op];
    if (handler) {
        handler(cpu, opcode);
    }

    //Mise à jour de l'état précédent du clavier 
    memcpy(cpu->key_prev, curr, sizeof(curr));
}

/**
 * @brief Met à jour les timers du processeur.
 * 
 * Décrémente DT et ST à 60 Hz et gère l'activation du speaker.
 * 
 * @param cpu Pointeur vers le processeur.
 */
void processor_update_timer(struct processor *cpu) {
    Uint32 now = SDL_GetTicks();

    //60 Hz
    if (now - cpu->dt_last_update >= 1000 / 60) {
        cpu->draw_allowed = 1;
        cpu->stalled = 0;

        //Décrémentation du timer DT
        if (cpu->DT > 0) {
            cpu->DT--;
        }
        
        //Décrémentation du tiùer ST et gestion du speaker
        if (cpu->ST > 0) {
            cpu->ST--;
            if (cpu->Speaker) {
                Speaker_on(cpu->Speaker);
            }
        } else {
            if (cpu->Speaker) {
                Speaker_off(cpu->Speaker);
            }
        }

        cpu->dt_last_update = now;
    }
}

