/**
 * @file opcode.h
 * @brief Déclaration des handlers d'opcodes Chip-8 et table de dispatch.
 * 
 * Ce fichier contient les prototypes des focntions responsables des instructions Chip-8.
 * Il contient aussi la table de dispatch permettant d'associer les opcodes à leur handler correspondant.
 */
#ifndef OPCODE_H
#define OPCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "processor.h"
#include "memory.h"

/**
 * @brief Adresse de départ des sprites de police (fonts) en mémoire.
 */
#define START_FONT_ADRESS 0x000

/**
 * @brief Type représentant un handler d'opcode Chip-8.
 * 
 * Chaque handler reçoit :
 * - un pointeur vers le processeur.
 * -l'opcode 16 bits à exécuter.
 */
typedef void (*opcode_handler)(struct processor *cpu, uint16_t opcode) ;

/**
 * @brief Instructions système (0x0NNN).
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_0(struct processor *cpu, uint16_t opcode);

/**
 * @brief Saut à l'adresse NNN.
 * @opcode 1nnn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_1(struct processor *cpu, uint16_t opcode);

/**
 * @brief Appel de sous-programme.
 * @opcode : 2nnn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_2(struct processor *cpu, uint16_t opcode);

/**
 * @brief Saute l'instruction si VX==KK.
 * @opcode 3xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_3(struct processor *cpu, uint16_t opcode);

/**
 * @brief Saute l'instruction si Vx!=KK.
 * @opcode 4xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_4(struct processor *cpu, uint16_t opcode);

/**
 * @brief Saute l'instruction si Vx==Vy.
 * @opcode 5xy0
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_5(struct processor *cpu, uint16_t opcode);

/**
 * @brief Charge kk dans Vx.
 * @opcode 6xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_6(struct processor *cpu, uint16_t opcode);

/**
 * @brief Ajoute kk à Vx.
 * @opcode 7xkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_7(struct processor *cpu, uint16_t opcode);

/**
 * @brief Instruction arithmétiques et logiques.
 * @opcode 8xyN
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_8(struct processor *cpu, uint16_t opcode);

/**
 * @brief Saute l'instruction si Vx!=Vy.
 * @opcode 9xy0
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_9(struct processor *cpu, uint16_t opcode);

/**
 * @brief Charge une adresse dans le registre I.
 * @opcode Annn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_A(struct processor *cpu, uint16_t opcode);

/**
 * @brief Saut à l'adresse nnn+V0
 * @opcode Bnnn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_B(struct processor *cpu, uint16_t opcode);

/**
 * @brief Génère un nombre aléatoire masqué.
 * @opcode Cxkk
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_C(struct processor *cpu, uint16_t opcode);

/**
 * @brief Dessine un sprite à l'écran.
 * @opcode Dxyn
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_D(struct processor *cpu, uint16_t opcode);

/**
 * @brief Instructions liées au clavier.
 * @opcode Ex9E/ExA1
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_E(struct processor *cpu, uint16_t opcode);

/**
 * @brief Instructions diverses.
 * @opcode Fx**
 * 
 * @param cpu Processeur
 * @param opcode Opcode à exécuter
 */
void op_F(struct processor *cpu, uint16_t opcode);

/**
 * @brief Table de dispatch des opcodes Chip-8.
 */
extern opcode_handler dispatch[16];

#endif