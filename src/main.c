
#include <string.h>
#include <stdio.h>
#include "memory.h"
#include "processor.h"
#include "display/display.h"
#include "misc/debug.h"
#include "keyboard/keyboard.h"
#include "speaker/speaker.h"

int main(void) {
    struct memory mem = {0};


    if (memory_load_rom(&mem, "c8games/VBRIX", START_ADDRESS) <= 0) {
        fprintf(stderr, "Erreur: chargement ROM\n");
        return 1;
    }

    struct Display dsp;
    if (Display_init(&dsp, 10) != 0) {
        fprintf(stderr, "Erreur: init Display (SDL)\n");
        return 1;
    }
    (void)Display_set_colors(&dsp, 255, 200, 255, 255, 38, 227);

    struct Keyboard kb;
    if (Keyboard_init(&kb) != 0) {
        fprintf(stderr, "Erreur: init Keyboard (SDL)\n");
        return 1;
    }
    
    struct Speaker spk;
    if (Speaker_init(&spk) !=0) {
        fprintf(stderr, "Erreur : init speaker\n");
    }

    struct processor cpu;
    if (processor_init(&cpu, &mem, &dsp, &kb, &spk) != 0) {
        fprintf(stderr, "Erreur: init CPU\n");
        Display_destroy(&dsp);
        Keyboard_destroy(&kb);
        Speaker_destroy(&spk);
        return 1;
    }


    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        processor_step(&cpu);
        Display_update(&dsp);
        processor_update_timer(&cpu);
        SDL_Delay(1);
    }

    Display_destroy(&dsp);
    Keyboard_destroy(&kb);
    Speaker_destroy(&spk);

    return 0;

}


