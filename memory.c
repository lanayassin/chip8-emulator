#include "memory.h"
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

int memory_load(struct memory* mem, const char* filename){
    if (mem==NULL || filename==NULL) return -1;
    FILE* f = fopen(filename, "rb");
    if (f==NULL) return -1;
    size_t space = MEMORY_SIZE - START_ADDRESS;
    size_t nread = fread(&mem->data[START_ADDRESS], 1, space, f);
    fclose(f);
    return (int)nread;



}