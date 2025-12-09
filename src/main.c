#include "include/memory.h"
#include "include/misc/debug.h"


int main(int argc, char** argv) {
    struct memory mem = {0};

    int bytes_loaded = memory_load_rom(&mem, "1-chip8-logo.ch8", START_ADDRESS);
    if (bytes_loaded < 0) {
        printf("Erreur lors du chargement de la ROM\n");
        return 1;
    }

    uint16_t instruction = memory_read_instruction(&mem, START_ADDRESS );
    printf("Instruction à 0x%X : 0x%X\n", START_ADDRESS, instruction);
    printf(instruction_as_str(instruction));
    return 0;
}

