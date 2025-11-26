#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define MEMORY_SIZE 4096
#define START_ADDRESS 0x200


struct memory {
    uint8_t data[MEMORY_SIZE];
};

int memory_write(struct memory* mem, uint16_t address, uint8_t value);

int memory_read( struct memory* mem, uint16_t address, uint8_t* out);

int memory_load(struct memory* mem, const char* filename);


#endif
