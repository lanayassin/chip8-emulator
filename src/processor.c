
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

    cpu->waiting_reg    = 0xFF;
    cpu->key_wait_phase = KEY_WAIT_NONE;
    cpu->latched_key    = -1;

    memset(cpu->key_prev, 0, sizeof(cpu->key_prev));

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



static inline void keyboard_snapshot(struct processor *cpu, uint8_t curr[16]) {
    for (uint8_t k = 0; k < 16; ++k) {
        int state = KEY_UP;
        if (cpu->Keyboard && Keyboard_get(cpu->Keyboard, k, &state) == 0) {
            curr[k] = (state == KEY_DOWN) ? 1 : 0;
        } else {
            curr[k] = 0;
        }
    }
}


void processor_step(struct processor *cpu) {
    assert(cpu && cpu->RAM);

    uint8_t curr[16];
    keyboard_snapshot(cpu, curr);

    if (cpu->key_wait_phase != KEY_WAIT_NONE) {
        if (cpu->key_wait_phase == KEY_WAIT_PRESS) {
            for (uint8_t k = 0; k < 16; ++k) {
                if (curr[k] && !cpu->key_prev[k]) {
                    cpu->V[cpu->waiting_reg] = k;  
                    cpu->latched_key = k;
                    cpu->key_wait_phase = KEY_WAIT_RELEASE;
                    break;
                }
            }
            if (cpu->key_wait_phase == KEY_WAIT_PRESS) {
                memcpy(cpu->key_prev, curr, sizeof(curr));
                return; 
            }
        }

        if (cpu->key_wait_phase == KEY_WAIT_RELEASE) {
            int lk = cpu->latched_key;
            if (lk >= 0) {

                if (!curr[lk] && cpu->key_prev[lk]) {
                    cpu->key_wait_phase = KEY_WAIT_NONE; 
                } else {
                    memcpy(cpu->key_prev, curr, sizeof(curr));
                    return; 
                }
            } else {

                cpu->key_wait_phase = KEY_WAIT_PRESS;
                memcpy(cpu->key_prev, curr, sizeof(curr));
                return;
            }
        }
    }

    uint16_t opcode = processor_fetch(cpu);
    uint8_t op = (opcode & 0xF000) >> 12;

    opcode_handler handler = dispatch[op];
    if (handler) {
        handler(cpu, opcode);
    }

    memcpy(cpu->key_prev, curr, sizeof(curr));
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

