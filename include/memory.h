/**
 * @file memory.h
 * @brief Gestion de la mémoire de l'émulateur Chip-8.
 * 
 * Ce fichier contient les définitions des constantes, structures et prototypes nécessaires à la gestion de la mémoire de l'émulateur Chip-8.
 * Il est utilisé par les différents modules de l'application.
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

/**
 * @def MEMORY_SIZE
 * @brief Taille totale de la mémoire Chip-8.
 */
#define MEMORY_SIZE 4096

/**
 * @def START_ADRESS
 * @brief Adresse de départ pour le chargement des ROM.
 */
#define START_ADDRESS 0x200

/**
 * @struct memory
 * @brief Structure représentant la mémoire.
 * 
 * Contient un tableau d'octets représentant la mémoire du système.
 */
struct memory {
    uint8_t data[MEMORY_SIZE];
};

/**
 * @brief Ecrit une valeur en mémoire à une adresse donnée.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param address Adresse mémoire où écrire la valeur.
 * @param value Valeur à écrire.
 * @return 0 en cas de succés, -1 en cas d'erreur.
 */
int memory_write(struct memory* mem, uint16_t address, uint8_t value);

/**
 * @brief Lit une valeur en mémoire à une adresse donnée.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param address Adresse mémoire à lire.
 * @param out Pointeur où stocker la valeur lue.
 * @return 0 en cas de succés, -1 en cas d'erreur.
 */
int memory_read(struct memory* mem, uint16_t address, uint8_t* out);

/**
 * @brief Charge une ROM en mémoire à partir d'un fichier.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param filename Chemin vers le fichier ROM.
 * @param start_address Adresse de départ en mémoire.
 * @return Nombre d'octets chargés en cas de succés, -1 en cas d'erreur.
 */
int memory_load_rom(struct memory* mem, const char* filename, uint16_t start_address);

/**
 * @brief Lit une instruction CHIP-8 en mémoire.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param addr Adresse de l'instruction.
 * @return Instruction 16 bits, ou -1 en cas d'erreur
 */
int memory_read_instruction(struct memory* mem, uint16_t addr);

#endif
