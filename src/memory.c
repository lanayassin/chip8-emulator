/**
 * @file memory.c
 * @brief Gestion de la mémoire de l'émulateur Chip-8.
 * 
 * Ce fichier contient les fonctions permettant de lire, écrire et charger des données en mémoire.
 * Il contient aussi la lecture des instructions Chip-8.
 */

#include <string.h>
#include <stdio.h>

#include "memory.h"

/**
 * @brief Ecrit une valeur en mémoire à une adresse donnée.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param address Adresse mémoire où écrire la valeur.
 * @param value Valeur à écrire.
 * @return 0 en cas de succés, -1 en cas d'erreur.
 */
int memory_write(struct memory* mem, uint16_t address, uint8_t value) {
    //Vérification de la validité de la mémoire et de l'adresse
    if (mem == NULL || address >= MEMORY_SIZE) return -1;

    //Ecriture de la valeur à l'adresse donnée
    mem->data[address] = value;

    return 0;
}

/**
 * @brief Lit une valeur en mémoire à une adresse donnée.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param address Adresse mémoire à lire.
 * @param out Pointeur où stocker la valeur lue.
 * @return 0 en cas de succés, -1 en cas d'erreur.
 */
int memory_read(struct memory* mem, uint16_t address, uint8_t* out) {
    //Vérification de la validité des paramètres
    if (mem == NULL || address >= MEMORY_SIZE || out == NULL) return -1;

    //Lecture de la valeur stockée à l'adresse donnée
    *out = mem->data[address];

    return 0;
}

/**
 * @brief Charge une ROM en mémoire à partir d'un fichier.
 * 
 * Le contenu du fichier est chargé en mémoire à partir de l'addresse donnée.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param filename Chemin vers le fichier ROM.
 * @param start_address Adresse de départ en mémoire.
 * @return Nombre d'octets chargés en cas de succés, -1 en cas d'erreur.
 */
int memory_load_rom(struct memory* mem, const char* filename, uint16_t start_address) {
    //Vérification de la validité des paramètres
    if (!mem || !filename) return -1;

    //Vérification de l'adresse de départ
    if (start_address >= MEMORY_SIZE) return -1;

    //Ouverture du fichier ROM en mode binaire
    FILE* f = fopen(filename, "rb");
    if (!f) return -1;

    //Calcul de l'espace disponible en mémoire
    size_t space = MEMORY_SIZE - start_address;

    //Chargement du contenu de la ROM en mémoire
    size_t nread = fread(&mem->data[start_address], 1, space, f);
    fclose(f);

    //Vérification de la validité de la lecture des données
    if (nread == 0) return -1;
    
    return (int)nread;
}

/**
 * @brief Lit une instruction CHIP-8 en mémoire.
 * 
 * @param mem Pointeur vers la structure mémoire.
 * @param addr Adresse de l'instruction.
 * @return Instruction 16 bits, ou -1 en cas d'erreur
 */
int memory_read_instruction(struct memory* mem, uint16_t addr) {
    //Vérification de la validité de la mémoire et de l'adresse
    if (!mem || addr + 1 >= MEMORY_SIZE) return -1;

    //Lecture de deux octets consécutifs pour former une instruction 6 bits
    return (uint16_t)(mem->data[addr] << 8) | mem->data[addr + 1];
}
