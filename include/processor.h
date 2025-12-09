#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "memory.h"
#include "display/display.h"

#define NB_REGISTERS 16

struct processor {
    const char Vx[NB_REGISTERS];
    uint16_t I;
    uint16_t PC;
    uint8_t DT;
    uint8_t ST;
    uint8_t SP;    
    struct memory * RAM;

};

#endif