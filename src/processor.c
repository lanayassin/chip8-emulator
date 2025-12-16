
#include "processor.h"
#include "memory.h"
#include "opcode.h"
#include "display/sprite.h"
#include "display/display.h"
#include "keyboard/keyboard.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

int processor_init(struct processor *cpu, struct memory* ram, struct Display* display, struct Keyboard* Keyboard, struct Speaker* Speaker) {
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
    cpu->Keyboard = Keyboard;
    cpu->Speaker = Speaker;

    cpu->dt_last_update = SDL_GetTicks();
    cpu->waiting_for_key = 0;
    cpu->waiting_reg = 0;
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
    assert(cpu && cpu->RAM);

    if (check_pc2(cpu) != 0) {
        fprintf(stderr, "PC out of bounds before fetch: PC=0x%03X\n", cpu->PC);
        return 0;
    }

    uint16_t opcode = memory_read_instruction(cpu->RAM, cpu->PC);
    cpu->PC += 2; 
    return opcode;
}


void processor_step(struct processor *cpu) {
    assert(cpu && cpu->RAM);
    
    if (cpu->waiting_for_key) {
    for (uint8_t k = 0; k < 16; ++k) {
        int state;
        if (Keyboard_get(cpu->Keyboard, k, &state) == 0 && state == KEY_DOWN) {
            cpu->V[cpu->waiting_reg] = k;
            cpu->waiting_for_key = 0;
            cpu->PC += 2; 
            break;
        }
    }
    return; 
    }
    uint16_t opcode = processor_fetch(cpu);
    uint8_t op = (opcode & 0xF000) >> 12;

    opcode_handler handler = dispatch[op];
    if (handler) {
        handler(cpu, opcode);
    }
}

void processor_update_timer(struct processor *cpu) {
    Uint32 now = SDL_GetTicks();
    if (now - cpu->dt_last_update >= 1000 / 60) { 
        if (cpu->DT > 0) cpu->DT--;

        if (cpu->ST > 0) {
            cpu->ST--;
            if (cpu->Speaker) {
                Speaker_on(cpu->Speaker);
            }
        } else {
            if (cpu->Speaker) {
                Speaker_off(cpu->Speaker);
            }
        }

        cpu->dt_last_update = now;
    }
}

