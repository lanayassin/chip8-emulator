#include "opcode.h"
#include "processor.h"
#include "memory.h"
#include <stdlib.h>
#include "keyboard/keyboard.h"
#include <stdlib.h>  
#include <time.h>
#include <string.h>

void (*dispatch[16])(struct processor *cpu, uint16_t opcode) = {
    op_0, op_1, op_2, op_3,
    op_4, op_5, op_6, op_7,
    op_8, op_9, op_A, op_B,
    op_C, op_D, op_E, op_F
};

// 0x0000 : Instructions Système
void op_0(struct processor *cpu, uint16_t opcode) {
    if (opcode == 0x00E0 && cpu->Display) {
        Display_CLS(cpu->Display);
        return;
    }
    if (opcode == 0x00EE) {
        if (cpu->SP == 0) {
            fprintf(stderr, "Stack underflow at RET!\n");
            exit(1);
        }
        cpu->PC = cpu->stack[--cpu->SP];
        return;
    }
}

// 0x1nnn : JP addr
void op_1(struct processor *cpu , uint16_t opcode) {
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->PC = adrr;
}

// 0x2nnn : CALL addr
void op_2(struct processor *cpu , uint16_t opcode){
    if (cpu->SP >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow at CALL!\n");
        exit(1);
    }
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->stack[cpu->SP++] = cpu->PC;
    cpu->PC = adrr;
}

// 0x3xkk : SE Vx, byte 
void op_3(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = opcode & 0x00FF;
    if (cpu->V[x]== kk){
        cpu->PC += 2 ;
    }
}

//4xkk : SNE Vx, byte
void op_4(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = opcode & 0x00FF;
    if (cpu->V[x] != kk){
        cpu->PC += 2 ;
    }
}

// 5xy0 : SE Vx, Vy
void op_5(struct processor *cpu , uint16_t opcode){
    if ((opcode & 0x000F) == 0x0) {
        uint8_t x = (opcode & 0x0F00) >> 8 ;
        uint8_t y = (opcode & 0x00F0) >> 4 ;
        if (cpu->V[x] == cpu->V[y]){
            cpu->PC += 2 ;
        }
    }

    
}

// 6xkk : LD Vx, byte 
void op_6(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = opcode & 0x00FF;
    cpu->V[x] = kk ;
}

// 7xkk : ADD Vx, byte
void op_7(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = opcode & 0x00FF;
    cpu->V[x] += kk ;
}

// 8xyN
void op_8(struct processor *cpu, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F);

    // 8xy0 : LD Vx, Vy
    if (n == 0x0) {
        cpu->V[x] = cpu->V[y];
    }
    // 8xy1 : OR Vx, Vy
    else if (n == 0x1) {
        cpu->V[x] = cpu->V[x] | cpu->V[y];
        cpu->V[0xF] = 0;
    }
    // 8xy2 : AND Vx, Vy
    else if (n == 0x2) {
        cpu->V[x] = cpu->V[x] & cpu->V[y];
        cpu->V[0xF] = 0;
    }
    // 8xy3 : XOR Vx, Vy
    else if (n == 0x3) {
       cpu->V[x] = cpu-> V[x] ^ cpu->V[y];
       cpu->V[0xF] = 0;
    }
    // 8xy4 : ADD Vx, Vy (Attention au Carry Flag VF)
    else if (n == 0x4) {
        uint16_t resultat = cpu->V[x] +cpu->V[y];
        if (resultat > 255) {
        cpu->V[x] = resultat;
        cpu->V[0xF] = 1;
        }
        else{
        cpu->V[x] = resultat;
        cpu->V[0xF] = 0;
        }

   
    }
    // 8xy5 : SUB Vx, Vy (VF = NOT Borrow)
    else if (n == 0x5) {
        if (cpu->V[x] >= cpu->V[y]) {
            cpu->V[x] -= cpu->V[y];
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x] -= cpu->V[y];
            cpu->V[0xF] = 0;
        }
    }
    // 8xy6 : SHR Vx (Décalage à droite)
    else if (n == 0x6) {
        if (cpu->V[x] & 0x1) {
            cpu->V[x] = cpu->V[y] >> 1;
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x] = cpu->V[y] >> 1;
            cpu->V[0xF] = 0;
        }
    }
    // 8xy7 : SUBN Vx, Vy (Vy - Vx)
    else if (n == 0x7) {
        if (cpu->V[y] >= cpu->V[x]) {
            cpu->V[x] = cpu->V[y] - cpu->V[x];
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x] = cpu->V[y] - cpu->V[x];
            cpu->V[0xF] = 0;
        }
    }
    // 8xyE : SHL Vx (Décalage à gauche)
    else if (n == 0xE) {
        if (cpu->V[x] & 0x80) {
            cpu->V[x] = cpu->V[y] << 1;
            cpu->V[0xF] = 1;
        } else {
            cpu->V[x]= cpu->V[y] << 1;
            cpu->V[0xF] = 0;
        }
        //cpu->V[0xF] = (cpu->V[x] & 0x80) >> 7; // On sauve le bit de poids fort (MSB)
        //cpu->V[x] <<= 1;
    }
}

//9xy0 : SNE Vx, Vy
void op_9 (struct processor *cpu , uint16_t opcode){
    if ((opcode & 0x000F) == 0x0) {
        uint8_t x = (opcode & 0x0F00) >> 8 ;
        uint8_t y = (opcode & 0x00F0) >> 4 ;
        if (cpu->V[x] != cpu->V[y]){
            cpu->PC += 2 ;
        }
    }
}

//Annn : LD I, addr
void op_A(struct processor *cpu , uint16_t opcode){
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->I = adrr ; 
}

//Bnnn : JP V0, addr
void op_B(struct processor *cpu , uint16_t opcode){
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->PC = adrr + cpu->V[0];
}

//Cxkk - RND Vx, byte
void op_C(struct processor *cpu , uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = (opcode & 0x00FF);

    uint8_t rnd = (uint8_t)(rand() % 256);
    
    cpu->V[x] = rnd & kk;   
}
//Dxyn - DRW Vx, Vy, nibble
void op_D(struct processor *cpu, uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t y = (opcode & 0x00F0) >> 4 ;
    uint8_t n = (opcode & 0x000F);
    struct Sprite spr;
    if (Sprite_init(&spr, n) != 0) return;

    for (uint8_t i = 0; i < n; ++i) {
        uint8_t byte = 0;
        memory_read(cpu->RAM, cpu->I + i, &byte);
        Sprite_add(&spr, byte);
    }
    if (!cpu->draw_allowed) {
        cpu->PC -= 2;
        cpu->stalled = 1; 
        Sprite_destroy(&spr);
        return;
    }

    cpu->draw_allowed = 0;

    uint8_t VF = 0;
    if (cpu->Display) {
        Display_DRW(cpu->Display, &spr, cpu->V[x], cpu->V[y], &VF);
    }
    cpu->V[0xF] = VF ? 1 : 0;
    Sprite_destroy(&spr);
}

void op_E(struct processor *cpu, uint16_t opcode){
    uint8_t x  = (opcode & 0x0F00) >> 8;
    uint8_t nn = (opcode & 0x00FF);
    int state;

    if (nn==0xA1) {
        if (Keyboard_get(cpu->Keyboard, cpu->V[x], &state)==0){
            if (state==KEY_UP) {
                cpu->PC += 2;
            }
        }
    }

    if (nn==0x9E) {
        if (Keyboard_get(cpu->Keyboard, cpu->V[x], &state)==0) {
            if (state == KEY_DOWN) {
                cpu->PC += 2;
            }
        }
    }
}

void op_F(struct processor *cpu, uint16_t opcode) {
    uint8_t x  = (opcode & 0x0F00) >> 8;
    uint8_t nn = (opcode & 0x00FF);

    if (nn == 0x07) {
        cpu->V[x] = cpu->DT;
    }

    if (nn == 0x0A) {
        cpu->waiting_reg    = x;
        cpu->latched_key    = -1;
        cpu->key_wait_phase = KEY_WAIT_PRESS;
        return;

    }

    if (nn == 0x15) {
        cpu->DT = cpu->V[x];
    }

    if (nn == 0x18) {
        cpu->ST = cpu->V[x];
    }

    if (nn == 0x1E) {
        cpu->I += cpu->V[x];
    }

    if (nn == 0x29) {
        cpu->I = (START_FONT_ADRESS) + (cpu->V[x] * 5);
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
        cpu->I += x + 1;
    }
 
    if (nn == 0x65) {
        for (uint8_t i = 0; i <= x; i++) {
            memory_read(cpu->RAM, cpu->I + i, &cpu->V[i]);
        }
        cpu->I += x + 1;
    }
}


