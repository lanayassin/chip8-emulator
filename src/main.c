/**
 * @file main.c
 * @brief Point d'entrée principal de l'émulateur Chip-8.
 * 
 * Ce fichier initialise les différents modules de l'émulateur :
 * -mémoire
 * -processeur
 * -affichage (SDL)
 * -clavier
 * -son
 * -interface utilisateur
 * 
 * Il gère également la boucle principale, les événements SDL et l'exécution de l'émulation Chip-8.
 */
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
#include "interface.h"

/**
 * @brief Fonction principale du programme.
 * 
 * Initialise SDL, les modules de l'émulateur et lance la boucle principale.
 * Gère les différents écrans :
 * - Menu principal
 * - Sélection de ROM
 * - Emulation CHip-8
 * 
 * @return 0 en cas de succés, 1 en cas d'erreur
 */
int main(void) {
    struct memory mem = {0};

    /* Initialisation SDL */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    const char* selectedRom = NULL;

    /* Initialisation de SDL_ttf */
    TTF_Init();
    TTF_Font* fontMenu  = TTF_OpenFont("fonts/Comfortaa-VariableFont_wght.ttf", 32);
    TTF_Font* fontList  = TTF_OpenFont("fonts/Comfortaa-VariableFont_wght.ttf", 16);

    if (!fontMenu || !fontList) {
        fprintf(stderr, "Erreur TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }

    /* Création de la fenêtre du menu */
    SDL_Window* menuWindow = SDL_CreateWindow("CHIP-8 Menu",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,600, 400, SDL_WINDOW_SHOWN );

    SDL_Renderer* menuRenderer = SDL_CreateRenderer(menuWindow, -1, SDL_RENDERER_ACCELERATED);

    /* Initialisation de l'affichage Chip-8 */
    struct Display dsp;
    if (Display_init(&dsp, 10) != 0) {
        fprintf(stderr, "Erreur: init Display (SDL)\n");
        return 1;
    }
    (void)Display_set_colors(&dsp, 255, 200, 255, 255, 38, 227);

    /* Initialisation du clavier */
    struct Keyboard kb;
    if (Keyboard_init(&kb) != 0) {
        fprintf(stderr, "Erreur: init Keyboard (SDL)\n");
        return 1;
    }
    
    /* Initialisation du haut-parleur */
    struct Speaker spk;
    if (Speaker_init(&spk) !=0) {
        fprintf(stderr, "Erreur : init speaker\n");
    }

    /* Initialisation du processeur*/
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

    /**
     * @brief Boucle principale de l'application.
     * 
     * Gère les événements SDL, l'affichage des menus et l'exécution de l'émulation Chip-8.
     */
    while (running) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            while (SDL_PollEvent(&event)) {

                /* Gestion de la fermeture des fenêtres */
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE ) {
                    Uint32 window_id = event.window.windowID;
                    if (window_id == SDL_GetWindowID(dsp.window)) {
                    Display_CLS(&dsp);          
                    SDL_HideWindow(dsp.window);
                    screen = SCREEN_ROM_SELECT;
                    emuReady = 0;
                    continue; 
                    } else {
                        running = 0;
                        continue;
                    }

                }
                /* Gestion des événements selon l'écran actif */
                if (screen == SCREEN_MENU) {
                    handle_menu_events(&screen, &running, event);
                }
                if (screen == SCREEN_ROM_SELECT) {
                    handle_select_events(&screen, event, &selectedRom);

                }
            }
            
            /* Affichage du menu principal */
            if (screen == SCREEN_MENU) {
                SDL_HideWindow(dsp.window);
                render_menu(menuRenderer, fontMenu, mouseX, mouseY);
            }

            /* Affichage de la sélection de ROM */
            else if (screen==SCREEN_ROM_SELECT) {
                SDL_HideWindow(dsp.window);
                render_select(menuRenderer, fontList, mouseX, mouseY);
            }

            /* Emulation Chip-8 */
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

        /* Libération des ressources*/
        Display_destroy(&dsp);
        Keyboard_destroy(&kb);
        Speaker_destroy(&spk);
        TTF_CloseFont(fontMenu);
        TTF_CloseFont(fontList);
        SDL_DestroyRenderer(menuRenderer);
        SDL_DestroyWindow(menuWindow);
        TTF_Quit();
        SDL_Quit();

        return 0;
    }


