
#ifndef OPCODE_H
#define OPCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "processor.h"
#include "memory.h"

#define START_FONT_ADRESS 0x000

typedef void (*opcode_handler)(struct processor *cpu, uint16_t opcode) ;

void op_0(struct processor *cpu, uint16_t opcode);
void op_1(struct processor *cpu, uint16_t opcode);
void op_2(struct processor *cpu, uint16_t opcode);
void op_3(struct processor *cpu, uint16_t opcode);
void op_4(struct processor *cpu, uint16_t opcode);
void op_5(struct processor *cpu, uint16_t opcode);
void op_6(struct processor *cpu, uint16_t opcode);
void op_7(struct processor *cpu, uint16_t opcode);
void op_8(struct processor *cpu, uint16_t opcode);
void op_9(struct processor *cpu, uint16_t opcode);
void op_A(struct processor *cpu, uint16_t opcode);
void op_B(struct processor *cpu, uint16_t opcode);
void op_C(struct processor *cpu, uint16_t opcode);
void op_D(struct processor *cpu, uint16_t opcode);
void op_E(struct processor *cpu, uint16_t opcode);
void op_F(struct processor *cpu, uint16_t opcode);

extern opcode_handler dispatch[16];

#endif


opcode_handler dispatch[16] = {
    op_0, op_1, op_2, op_3, op_4, op_5, op_6, op_7,
    op_8, op_9, op_A, op_B, op_C, op_D, op_E, op_F
};


void op_F(struct processor *cpu, uint16_t opcode) {
    uint8_t x  = (opcode & 0x0F00) >> 8;
    uint8_t nn = (opcode & 0x00FF);

    if (nn == 0x07) {
        cpu->V[x] = cpu->DT;
    }

    if (nn == 0x0A) {
        int key = keyboard_get_pressed(cpu->Keyboard);

        if (key<0) {
            cpu->PC -= 2;
        }
        else {
            cpu->V[x] = (uint8_t)key;
        }
    }

    if (nn == 0x15) {
        cpu->DT = V[x];
    }

    if (nn == 0x18) {
        cpu->ST = V[x];
    }

    if (nn == 0x1E) {
        cpu->I += cpu->V[x];
    }

    if (nn == 0x29) {
        cpu->I = (START_FONT_ADRESS) + (cpu->V[x] * 5)
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
    }
 
    if (nn == 0x65) {
        for (uint8_t i = 0; i <= x; i++) {
            memory_read(cpu->RAM, cpu->I + i, &cpu->V[i]);
        }
    }
}
