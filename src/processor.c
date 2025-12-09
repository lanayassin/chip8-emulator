#include "include/processor.h"
#include "include/memory.h"
#include <string.h>

int processor_init(struct processor *cpu, struct memory* ram, struct Display* display ) {
    memset(cpu, 0, sizeof(cpu));
    cpu->RAM = ram;
    cpu->Display = display;
    cpu->PC = START_ADDRESS;
}

void processor_destroy(struct processor *cpu) {
    free(cpu->RAM);
    free(cpu->Display);
    free(cpu->PC);
    free(cpu->DT);
    free(cpu->I);
    free(cpu->SP);
    free(cpu->ST);
    free(cpu->stack);
    free(cpu->V);
    free(cpu);
    return;
}
