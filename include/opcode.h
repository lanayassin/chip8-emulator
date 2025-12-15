
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