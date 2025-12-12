#include "opcode.h"
#include "processor.h"
#include "memory.h"

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

