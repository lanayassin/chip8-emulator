
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "memory.h"
#include "display/display.h"

#define NB_REGISTERS 16
#define STACK_SIZE 16

struct processor {
    uint8_t V[NB_REGISTERS];
    uint16_t I;
    uint16_t PC;
    uint8_t DT;
    uint8_t ST;
    uint8_t SP;
    uint16_t stack[STACK_SIZE];
    struct memory * RAM;
    struct Display * Display;
};

int processor_init(struct processor *cpu, struct memory* ram, struct Display* display);

void processor_destroy(struct processor *cpu);

uint16_t processor_fetch(struct processor *cpu);

void processor_step(struct processor *cpu);

#endif
