
#include <string.h>
#include <stdio.h>
#include "memory.h"
#include "processor.h"
#include "display/display.h"
#include "misc/debug.h"
#include "keyboard/keyboard.h"
#include "speaker/speaker.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <interface.h>


int main(void) {
    struct memory mem = {0};
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    const char* selectedRom = NULL;

    TTF_Init();  // Initialisation de SDL_ttf
    TTF_Font* fontMenu  = TTF_OpenFont("fonts/Comfortaa-VariableFont_wght.ttf", 32);
    TTF_Font* fontList  = TTF_OpenFont("fonts/Comfortaa-VariableFont_wght.ttf", 16);

    if (!fontMenu || !fontList) {
        fprintf(stderr, "Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* menuWindow = SDL_CreateWindow("CHIP-8 Menu",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,600, 400, SDL_WINDOW_SHOWN );

    SDL_Renderer* menuRenderer = SDL_CreateRenderer(menuWindow, -1, SDL_RENDERER_ACCELERATED);

    /*
    if (memory_load_rom(&mem, "c8games/VERS", START_ADDRESS) <= 0) {
        fprintf(stderr, "Erreur: chargement ROM\n");
        return 1;
    }
    */
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

    enum Screen screen = SCREEN_MENU;

    int emuReady = 0;

    while (running) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(dsp.window)) {
                    Display_CLS(&dsp);          // clear proprement
                    SDL_HideWindow(dsp.window);

                    screen = SCREEN_ROM_SELECT; // Retour au menu de sélection
                    emuReady = 0;
                    SDL_HideWindow(dsp.window);
                    continue; // passe au prochain événement
            }
                if (event.type == SDL_QUIT) {
                    if (screen==SCREEN_EMULATION) {
                        Display_CLS(&dsp);
                        SDL_HideWindow(dsp.window);
                        
                        screen = SCREEN_ROM_SELECT;
                        emuReady = 0;

                    }
                    else {
                        running = 0;
                    }
                }
                if (screen == SCREEN_MENU) {
                    handle_menu_events(&screen, &running, event);
                }
                if (screen == SCREEN_ROM_SELECT) {
                    handle_select_events(&screen, event, &selectedRom);

                }
            }

            if (screen == SCREEN_MENU) {
                SDL_HideWindow(dsp.window);
                render_menu(menuRenderer, fontMenu, mouseX, mouseY);
            }

            else if (screen==SCREEN_ROM_SELECT) {
                SDL_HideWindow(dsp.window);
                render_select(menuRenderer, fontList, mouseX, mouseY);
            }

            else if (screen == SCREEN_EMULATION) {

                SDL_ShowWindow(dsp.window);

                if (!emuReady) {
                    memset(&mem, 0, sizeof(mem));
                    if (selectedRom) {
                        memory_load_rom(&mem, selectedRom, START_ADDRESS);
                    }
                    processor_init(&cpu, &mem, &dsp, &kb, &spk);
                    emuReady = 1;
                }

                processor_step(&cpu);
                processor_update_timer(&cpu);
                Display_update(&dsp);
                SDL_Delay(1);
            }
        }

        Display_destroy(&dsp);
        Keyboard_destroy(&kb);
        Speaker_destroy(&spk);
        TTF_CloseFont(fontMenu);
        TTF_CloseFont(fontList);
        TTF_Quit();
        SDL_Quit();

        return 0;
    }


