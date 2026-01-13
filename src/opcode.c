/**
 * @file opcode.c
 * @brief Implémentation des instructions Chip-8.
 * 
 * Ce fichier contient l'implémentation de l'ensemble des opcodes Chip-8.
 * Il contient aussi la table de dispatch permettant leur exécution.
 */
#include "opcode.h"
#include "processor.h"
#include "memory.h"
#include "keyboard/keyboard.h"
#include <stdlib.h>  
#include <time.h>
#include <string.h>

/**
 * @brief Table de dispatch des opcodes.
 * 
 * Chaque entrée pointe vers la focntion chargée d'exécuter un opcode.
 */
void (*dispatch[16])(struct processor *cpu, uint16_t opcode) = {
    op_0, op_1, op_2, op_3,
    op_4, op_5, op_6, op_7,
    op_8, op_9, op_A, op_B,
    op_C, op_D, op_E, op_F
};

/**
 * @brief Instructions système (0x0NNN).
 * 
 * Gère :
 * - 00E0 : effacement de l'écran
 * - 00EE : retour de sous-programme
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_0(struct processor *cpu, uint16_t opcode) {
    if (opcode == 0x00E0 && cpu->Display) {
        Display_CLS(cpu->Display);
        return;
    }
    if (opcode == 0x00EE) {
        if (cpu->SP == 0) {
            fprintf(stderr, "Stack underflow at RET!\n");
            exit(1);
        }
        cpu->PC = cpu->stack[--cpu->SP];
        return;
    }
}

/**
 * @brief Saut à l'adresse NNN.
 * @opcode 1nnn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_1(struct processor *cpu , uint16_t opcode) {
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->PC = adrr;
}

/**
 * @brief Appel de sous-programme.
 * @opcode : 2nnn
 * 
 * Empile l'adresse courante puis saute à l'adresse donnée.
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_2(struct processor *cpu , uint16_t opcode){
    if (cpu->SP >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow at CALL!\n");
        exit(1);
    }
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->stack[cpu->SP++] = cpu->PC;
    cpu->PC = adrr;
}

/**
 * @brief Saute l'instruction si VX==KK.
 * @opcode 3xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_3(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = (uint8_t)(opcode & 0x00FF);
    if (cpu->V[x]== kk){
        cpu->PC += 2 ;
    }
}

/**
 * @brief Saute l'instruction si Vx!=KK.
 * @opcode 4xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_4(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = (uint8_t)(opcode & 0x00FF);
    if (cpu->V[x] != kk){
        cpu->PC += 2 ;
    }
}

/**
 * @brief Saute l'instruction si Vx==Vy.
 * @opcode 5xy0
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_5(struct processor *cpu , uint16_t opcode){
    if ((opcode & 0x000F) == 0x0) {
        uint8_t x = (opcode & 0x0F00) >> 8 ;
        uint8_t y = (opcode & 0x00F0) >> 4 ;
        if (cpu->V[x] == cpu->V[y]){
            cpu->PC += 2 ;
        }
    }
}

/**
 * @brief Charge kk dans Vx.
 * @opcode 6xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_6(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = (uint8_t)(opcode & 0x00FF);
    cpu->V[x] = kk ;
}

/**
 * @brief Ajoute kk à Vx.
 * @opcode 7xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_7(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = (uint8_t)(opcode & 0x00FF);
    cpu->V[x] += kk ;
}

/**
 * @brief Instruction arithmétiques et logiques.
 * @opcode 8xyN
 * 
 * Gère les opérations :
 * LD, OR, AND, XOR, ADD, SUB, SHR, SUBN, SHL
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_8(struct processor *cpu, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);

    // 8xy0 : LD Vx, Vy
    if (n == 0x0) {
        cpu->V[x] = cpu->V[y];
    }
    // 8xy1 : OR Vx, Vy
    else if (n == 0x1) {
        cpu->V[x] = cpu->V[x] | cpu->V[y];
        cpu->V[0xF] = 0;
    }
    // 8xy2 : AND Vx, Vy
    else if (n == 0x2) {
        cpu->V[x] = cpu->V[x] & cpu->V[y];
        cpu->V[0xF] = 0;
    }
    // 8xy3 : XOR Vx, Vy
    else if (n == 0x3) {
       cpu->V[x] = cpu-> V[x] ^ cpu->V[y];
       cpu->V[0xF] = 0;
    }
    // 8xy4 : ADD Vx, Vy (Attention au Carry Flag VF)
    else if (n == 0x4) {
        uint16_t resultat = cpu->V[x] +cpu->V[y];
        if (resultat > 255) {
        cpu->V[x] = (uint8_t)resultat;
        cpu->V[0xF] = 1;
        }
        else{
        cpu->V[x] = (uint8_t)resultat;
        cpu->V[0xF] = 0;
        }

   
    }
    // 8xy5 : SUB Vx, Vy (VF = NOT Borrow)
    else if (n == 0x5) {
        if (cpu->V[x] >= cpu->V[y]) {
            cpu->V[x] -= cpu->V[y];
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x] -= cpu->V[y];
            cpu->V[0xF] = 0;
        }
    }
    // 8xy6 : SHR Vx (Décalage à droite)
    else if (n == 0x6) {
        if (cpu->V[x] & 0x1) {
            cpu->V[x] = cpu->V[y] >> 1;
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x] = cpu->V[y] >> 1;
            cpu->V[0xF] = 0;
        }
    }
    // 8xy7 : SUBN Vx, Vy (Vy - Vx)
    else if (n == 0x7) {
        if (cpu->V[y] >= cpu->V[x]) {
            cpu->V[x] = cpu->V[y] - cpu->V[x];
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x] = cpu->V[y] - cpu->V[x];
            cpu->V[0xF] = 0;
        }
    }
    // 8xyE : SHL Vx (Décalage à gauche)
    else if (n == 0xE) {
        if (cpu->V[x] & 0x80) {
            cpu->V[x] = cpu->V[y] << 1;
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x]= cpu->V[y] << 1;
            cpu->V[0xF] = 0;
        }
    }
}

/**
 * @brief Saute l'instruction si Vx!=Vy.
 * @opcode 9xy0
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_9 (struct processor *cpu , uint16_t opcode){
    if ((opcode & 0x000F) == 0x0) {
        uint8_t x = (opcode & 0x0F00) >> 8 ;
        uint8_t y = (opcode & 0x00F0) >> 4 ;
        if (cpu->V[x] != cpu->V[y]){
            cpu->PC += 2 ;
        }
    }
}

/**
 * @brief Charge une adresse dans le registre I.
 * @opcode Annn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_A(struct processor *cpu , uint16_t opcode){
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->I = adrr ; 
}

/**
 * @brief Saut à l'adresse nnn+V0
 * @opcode Bnnn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_B(struct processor *cpu , uint16_t opcode){
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->PC = adrr + cpu->V[0];
}

/**
 * @brief Génère un nombre aléatoire masqué.
 * @opcode Cxkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_C(struct processor *cpu , uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = (uint8_t)(opcode & 0x00FF);

    uint8_t rnd = (uint8_t)(rand() % 256);
    
    cpu->V[x] = rnd & kk;   
}

/**
 * @brief Dessine un sprite à l'écran.
 * @opcode Dxyn
 * 
 * Met à jour le flag VF en cas de collision.
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_D(struct processor *cpu, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t y = (opcode & 0x00F0) >> 4 ;
    uint8_t n = (opcode & 0x000F);
    struct Sprite spr;
    if (Sprite_init(&spr, n) != 0) return;

    for (uint8_t i = 0; i < n; ++i) {
        uint8_t byte = 0;
        memory_read(cpu->RAM, cpu->I + i, &byte);
        Sprite_add(&spr, byte);
    }
    if (!cpu->draw_allowed) {
        cpu->PC -= 2;
        cpu->stalled = 1; 
        Sprite_destroy(&spr);
        return;
    }

    cpu->draw_allowed = 0;

    uint8_t VF = 0;
    if (cpu->Display) {
        Display_DRW(cpu->Display, &spr, cpu->V[x], cpu->V[y], &VF);
    }
    cpu->V[0xF] = VF ? 1 : 0;
    Sprite_destroy(&spr);
}

/**
 * @brief Instructions liées au clavier.
 * @opcode Ex9E/ExA1
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_E(struct processor *cpu, uint16_t opcode){
    uint8_t x  = (opcode & 0x0F00) >> 8;
    uint8_t nn = (uint8_t)(opcode & 0x00FF);
    int state;

    if (nn==0xA1) {
        if (Keyboard_get(cpu->Keyboard, cpu->V[x], &state)==0){
            if (state==KEY_UP) {
                cpu->PC += 2;
            }
        }
    }

    if (nn==0x9E) {
        if (Keyboard_get(cpu->Keyboard, cpu->V[x], &state)==0) {
            if (state == KEY_DOWN) {
                cpu->PC += 2;
            }
        }
    }
}

/**
 * @brief Instructions diverses.
 * @opcode Fx**
 * 
 * Gère timers, mémoire, clavier et BCD.
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_F(struct processor *cpu, uint16_t opcode) {
    uint8_t x  = (opcode & 0x0F00) >> 8;
    uint8_t nn = (uint8_t)(opcode & 0x00FF);

    if (nn == 0x07) {
        cpu->V[x] = cpu->DT;
    }

    if (nn == 0x0A) {
        cpu->waiting_reg    = x;
        cpu->latched_key    = -1;
        cpu->key_wait_phase = KEY_WAIT_PRESS;
        return;

    }

    if (nn == 0x15) {
        cpu->DT = cpu->V[x];
    }

    if (nn == 0x18) {
        cpu->ST = cpu->V[x];
    }

    if (nn == 0x1E) {
        cpu->I += cpu->V[x];
    }

    if (nn == 0x29) {
        cpu->I = (START_FONT_ADRESS) + (cpu->V[x] * 5);
    }

    if (nn == 0x33) {
        uint8_t value = cpu->V[x];
        memory_write(cpu->RAM, cpu->I, value / 100);
        memory_write(cpu->RAM, cpu->I + 1, (value / 10) % 10);
        memory_write(cpu->RAM, cpu->I + 2, value % 10);

    }

    if (nn == 0x55) {
        for (uint8_t i = 0; i <= x; i++) {
            memory_write(cpu->RAM, cpu->I + i, cpu->V[i]);
        }
        cpu->I += (uint16_t)(x + 1);
    }
 
    if (nn == 0x65) {
        for (uint8_t i = 0; i <= x; i++) {
            memory_read(cpu->RAM, cpu->I + i, &cpu->V[i]);
        }
        cpu->I += (uint16_t)(x + 1);
    }
}


