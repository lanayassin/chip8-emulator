
#include <string.h>
#include <stdio.h>
#include "memory.h"
#include "processor.h"
#include "display/display.h"

int main(void) {
    struct memory mem = {0};

    if (memory_load_rom(&mem, "./1-chip8-logo.ch8", START_ADDRESS) <= 0) {
        fprintf(stderr, "Erreur: chargement ROM\n");
        return 1;
    }

    struct Display dsp;
    if (Display_init(&dsp, 10) != 0) {
        fprintf(stderr, "Erreur: init Display (SDL)\n");
        return 1;
    }
    (void)Display_set_colors(&dsp, 0, 0, 0, 255, 255, 255);

    struct processor cpu;
    if (processor_init(&cpu, &mem, &dsp) != 0) {
        fprintf(stderr, "Erreur: init CPU\n");
        Display_destroy(&dsp);
        return 1;
    }

    for (int i = 0; i < 39; ++i) {
        processor_step(&cpu);
        (void)Display_update(&dsp);
    }

    Display_destroy(&dsp);
    return 0;
}
