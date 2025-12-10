
#include "processor.h"
#include "memory.h"
#include "display/sprite.h"
#include "display/display.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

int processor_init(struct processor *cpu, struct memory* ram, struct Display* display) {
    if (!cpu || !ram) return -1;
    memset(cpu->V, 0, sizeof(cpu->V));
    cpu->I = 0;
    cpu->PC = START_ADDRESS;

    cpu->DT = 0;
    cpu->ST = 0;


    cpu->SP = 0;
    memset(cpu->stack, 0, sizeof(cpu->stack));

    cpu->RAM     = ram;
    cpu->Display = display;
    return 0;
}


void processor_destroy(struct processor **cpu) {
    if (cpu) {
        *cpu = NULL;
    }
}


static inline int check_pc2(struct processor *cpu) {
    return (cpu->PC +1 <MEMORY_SIZE)?0 : 1;
}
uint16_t processor_fetch(struct processor *cpu) {
    assert(cpu&&cpu->RAM);
    if (check_pc2(cpu)!=0) {
        fprintf(stderr, "PC out of bounds before fetch: PC=0x%03X\n", cpu->PC);
        return 0;
    }
    uint16_t opcode = memory_read_instruction(cpu->RAM, cpu->PC);
    cpu->PC += 2;
    return opcode;
}

void processor_step(struct processor *cpu) {
    assert(cpu && cpu->RAM) ;
    uint16_t opcode = processor_fetch(cpu);

    uint8_t  op  = (opcode & 0xF000) >> 12;
    uint8_t  x   = (opcode & 0x0F00) >> 8;
    uint8_t  y   = (opcode & 0x00F0) >> 4;
    uint8_t  n   = (opcode & 0x000F);
    uint8_t  nn  = (opcode & 0x00FF);
    uint16_t nnn = (opcode & 0x0FFF);

    switch (op) {
        case 0x0: {
            if (opcode == 0x00E0 && cpu->Display) {
                (void)Display_CLS(cpu->Display);
            }
            break;
        }
        case 0x1: { 
            cpu->PC = nnn;
            break;
        }
        case 0x6: { 
            cpu->V[x] = nn;
            break;
        }
        case 0xA: { 
            cpu->I = nnn;
            break;
        }
        case 0xD: { 
            struct Sprite spr;
            if(Sprite_init(&spr,n)!=0) {
                break;
            }
            for (uint8_t i = 0; i < spr.length; ++i) {
                uint8_t b = 0;
                memory_read(cpu->RAM, (uint16_t)(cpu->I + i), &b);
                Sprite_add(&spr, b);
            }
            uint8_t VF = 0;
            if (cpu->Display) {
                (void)Display_DRW(cpu->Display, &spr, cpu->V[x], cpu->V[y], &VF);
            }
            cpu->V[0xF] = VF ? 1 : 0;
            break;
        }
        default:
            break;
    }
}
