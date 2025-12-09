#include "include/memory.h"
#include <string.h>

int memory_write(struct memory* mem, uint16_t address, uint8_t value){
    if (mem==NULL || address>=MEMORY_SIZE) return -1;
    mem->data[address]=value;
    return 0;
}


int memory_read(struct memory* mem, uint16_t address, uint8_t* out){
    if (mem==NULL || address>=MEMORY_SIZE) return -1;
    *out =  mem->data[address];
    return 0;
    
}
int memory_load_rom(struct memory* mem, const char* filename, uint16_t start_address) {
    if (!mem || !filename) return -1;

    FILE* f = fopen(filename, "rb");
    if (!f) return -1;

    size_t space = MEMORY_SIZE - start_address;
    size_t nread = fread(&mem->data[start_address], 1, space, f);

    fclose(f);
    return (int)nread;
}

uint16_t memory_read_instruction(struct memory* mem, uint16_t addr) {
    if (!mem || addr + 1 >= MEMORY_SIZE) return 0;
    return (mem->data[addr] << 8) | mem->data[addr + 1];
}

