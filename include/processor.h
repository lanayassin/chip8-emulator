/**
 * @file processor.h
 * @brief Déclaration du processeur Chip-8.
 * 
 * 
 * Ce fichier contient les définitions du processeur, de ses registres et des focntions permettant son initialisation, son exécution et la gestion des timers.
 * Il est utilisé par les différents modules de l'application.
 */
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "memory.h"
#include "display/display.h"

/**
 * @def NB_REGISTERS
 * @brief Nombre de registres généraux du processeur
 */
#define NB_REGISTERS 16

/**
 * @def STACK_SIZE
 * @brief Taille de la pile du processeur.
 */
#define STACK_SIZE 16

/**
 * @enum KeyWaitPhase
 * @brief Représente les différentes phases d'attente clavier.
 * 
 * Cette énumération est utilisée pour gérer l'instruction FX0A (attente de l'appui puis du relâchement d'une touche).
 */
typedef enum {
    KEY_WAIT_NONE    = 0, /**< Aucune attente clavier */
    KEY_WAIT_PRESS   = 1, /**< Attente de l'appui d'une touche */
    KEY_WAIT_RELEASE = 2 /**< Attente du relâchement de la touche */
} KeyWaitPhase;

/**
 * @struct processor
 * @brief Structure représenatnt l'état du processeur.
 * 
 * Cette structure contient l'ensemble des registres, la pile, les timers, ainsi que les pointeurs vers les périphériques.
 */
struct processor {
    uint8_t V[NB_REGISTERS]; /**< Registres généraux V0 à VF */
    uint16_t I; /**< Registre index */
    uint16_t PC; /**< Compteur ordinal */
    uint8_t DT; /**< Delay Timer */
    uint8_t ST; /**< Sound Timer */
    uint8_t SP; /**< Pointeur de la pile */

    uint16_t stack[STACK_SIZE]; /**< Pile d'adresses */

    struct memory * RAM; /**< Mémoire princpale */
    struct Display * Display; /**< Module d'affcihage */
    struct Keyboard * Keyboard; /**< Module Clavier */
    struct Speaker * Speaker; /**< Module Speaker */

    uint32_t dt_last_update; /**< Dernière mise à jour des timers */

    int  draw_allowed; /**< Autorise le dessin à l'écran */
    int stalled; /**< Indique si le processeur est bloqué */

    uint8_t     waiting_reg; /** Registre cible lors de l'attente clavier*/
    KeyWaitPhase key_wait_phase; /**< Phase actuelle d'attente clavier */
    int8_t      latched_key; /**< Touche actuellement mémorisée */

    uint8_t key_prev[16]; /**< Etat précédent des touches */


};

/**
 * @brief Initialise le processeur Chip-8.
 * 
 * @param cpu Pointeur vers la structure processeur
 * @param ram Mémoire principale
 * @param display Module d'affichage
 * @param Keyboard Module clavier
 * @param Speaker Module speaker
 * @return 0 en cas de succés, -1 en cas d'erreur
 */
int processor_init(struct processor *cpu, struct memory* ram, struct Display* display, struct Keyboard* Keyboard, struct Speaker* Speaker);

/**
 * @brief Détruit le processeur.
 * 
 * @param cpu Double pointeur vers la structure processeur
 */
void processor_destroy(struct processor **cpu);

/**
 * @brief Récupère l'instruction suivante en mémoire.
 * 
 * @param cpu Pointeur vers le processeur.
 * @return Opcode 16 bits, ou -1 en cas d'erreur.
 */
uint16_t processor_fetch(struct processor *cpu);

/**
 * @brief Exécute un cycle du processeur.
 * 
 * @param cpu Pointeur vers le processeur.
 */
void processor_step(struct processor *cpu);

/**
 * @brief Met à jour les timers du processeur.
 * 
 * @param cpu Pointeur vers le processeur.
 */
void processor_update_timer(struct processor *cpu);

#endif
