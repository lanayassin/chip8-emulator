#include "opcode.h"
#include "processor.h"
#include "memory.h"


opcode_handler dispatch[16] = {
    op_0, op_1, op_2, op_3, op_4, op_5, op_6, op_7,
    op_8, op_9, op_A, op_B, op_C, op_D, op_E, op_F
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
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->stack[cpu->SP++] = cpu->PC;
    cpu->PC = addr;
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
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t y = (opcode & 0x00F0) >> 4 ;
    if (cpu->V[x] == cpu->V[y]){
        cpu->PC += 2 ;
    }
}

// 6xkk : LD Vx, byte 
void op_6(struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = opcode & 0x00FF;
    cpu->V[x] == kk ;
}

// 7xkk : ADD Vx, byte
void op_7(stuct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t kk = opcode & 0x00FF;
    cpu->V[x] += kk ;
}

//8.. A FAIRE

//9xy0 : SNE Vx, Vy
void op_9 (struct processor *cpu , uint16_t opcode){
    uint8_t x = (opcode & 0x0F00) >> 8 ;
    uint8_t y = (opcode & 0x00F0) >> 4 ;
    if (cpu->V[x] != cpu->V[y]){
        cpu->PC += 2 ;
    }
}

//Annn : LD I, addr
void op_A(struct proccesor *cpu , uint16_t opcode){
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->I = adrr ; 
}

//Bnnn : JP V0, addr
void op_B(struct proccesor *cpu , uint16_t opcode){
    uint16_t adrr = opcode & 0x0FFF ;
    cpu->PC = adrr + cpu->V[0];
}

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


