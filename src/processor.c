
#include "processor.h"
#include "memory.h"
#include "display/sprite.h"
#include "display/display.h"
#include <string.h>

int processor_init(struct processor *cpu, struct memory* ram, struct Display* display) {
    if (!cpu || !ram) return -1;
    memset(cpu, 0, sizeof(*cpu));
    cpu->RAM     = ram;
    cpu->Display = display;
    cpu->PC      = START_ADDRESS;
    return 0;
}

void processor_destroy(struct processor *cpu) {
    (void)cpu;
}

uint16_t processor_fetch(struct processor *cpu) {
    uint16_t opcode = memory_read_instruction(cpu->RAM, cpu->PC);
    cpu->PC += 2;
    return opcode;
}

void processor_step(struct processor *cpu) {
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
            spr.length = n;
            for (uint8_t i = 0; i < spr.length; ++i) {
                uint8_t b = 0;
                memory_read(cpu->RAM, (uint16_t)(cpu->I + i), &b);
                spr.contents[i] = b;
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
