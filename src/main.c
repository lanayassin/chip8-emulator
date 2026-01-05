
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

enum Screen {
    SCREEN_MENU,
    SCREEN_ROM_SELECT,
    SCREEN_EMULATION
};

void handle_menu_events(enum Screen* screen, int* running, SDL_Event event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;

        SDL_Rect play = {180, 120, 240, 60};
        SDL_Rect quit = {180, 220, 240, 60};

        if (x >= play.x && x <= play.x + play.w &&
            y >= play.y && y <= play.y + play.h) {
            *screen = SCREEN_EMULATION;
        }

        if (x >= quit.x && x <= quit.x + quit.w &&
            y >= quit.y && y <= quit.y + quit.h) {
            *running = 0;
        }
    }
}

void render_menu(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY) {
    // Fond de la fenêtre
    SDL_SetRenderDrawColor(r, 255, 200, 255, 255); // rose clair
    SDL_RenderClear(r);

    // Définition des boutons
    SDL_Rect play = {180, 150, 240, 60};
    SDL_Rect quit = {180, 250, 240, 60};
    int border_thickness = 4;
    SDL_Color borderColor = {255, 38, 227, 255}; // violet

    // Couleurs pour chaque bouton
    SDL_Color playFill, playText;
    SDL_Color quitFill, quitText;

    // Hover Play
    if (mouseX >= play.x && mouseX <= play.x + play.w &&
        mouseY >= play.y && mouseY <= play.y + play.h) {
        playFill = (SDL_Color){255, 38, 227, 255};    // fond violet
        playText = (SDL_Color){255, 200, 255, 255};   // texte clair
    } else {
        playFill = (SDL_Color){255, 200, 255, 255};   // fond clair
        playText = (SDL_Color){255, 38, 227, 255};    // texte violet
    }

    // Hover Quit
    if (mouseX >= quit.x && mouseX <= quit.x + quit.w &&
        mouseY >= quit.y && mouseY <= quit.y + quit.h) {
        quitFill = (SDL_Color){255, 38, 227, 255};
        quitText = (SDL_Color){255, 200, 255, 255};
    } else {
        quitFill = (SDL_Color){255, 200, 255, 255};
        quitText = (SDL_Color){255, 38, 227, 255};
    }

    // Dessin Play
    SDL_SetRenderDrawColor(r, playFill.r, playFill.g, playFill.b, playFill.a);
    SDL_RenderFillRect(r, &play);
    SDL_SetRenderDrawColor(r, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i < border_thickness; i++) {
        SDL_Rect b = {play.x - i, play.y - i, play.w + 2*i, play.h + 2*i};
        SDL_RenderDrawRect(r, &b);
    }

    // Dessin Quit
    SDL_SetRenderDrawColor(r, quitFill.r, quitFill.g, quitFill.b, quitFill.a);
    SDL_RenderFillRect(r, &quit);
    SDL_SetRenderDrawColor(r, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i < border_thickness; i++) {
        SDL_Rect b = {quit.x - i, quit.y - i, quit.w + 2*i, quit.h + 2*i};
        SDL_RenderDrawRect(r, &b);
    }

    // Texte des boutons
    render_text(r, font, "PLAY", play, playText);
    render_text(r, font, "QUIT", quit, quitText);

    // Titre
    SDL_Rect titleRect = {0, 50, 600, 50};
    TTF_Font* titleFont = TTF_OpenFont("arial.ttf", 48);
    if (titleFont) {
        render_text(r, titleFont, "PolyChip8", titleRect, (SDL_Color){255,38,227,255});
        TTF_CloseFont(titleFont);
    }

    SDL_RenderPresent(r);
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW = 0, textH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);

    SDL_Rect dst = {
        rect.x + (rect.w - textW)/2,
        rect.y + (rect.h - textH)/2,
        textW,
        textH
    };

    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


int main(void) {
    struct memory mem = {0};
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

    TTF_Init();  // Initialisation de SDL_ttf
    TTF_Font* font = TTF_OpenFont("arial.ttf", 32); // mettre le chemin d'une police existante
    if (!font) {
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
                if (event.type == SDL_QUIT)
                    running = 0;

                if (screen == SCREEN_MENU) {
                    handle_menu_events(&screen, &running, event);
                }
            }

            if (screen == SCREEN_MENU) {
                SDL_HideWindow(dsp.window);
                render_menu(menuRenderer, font, mouseX, mouseY);
            }

            else if (screen == SCREEN_EMULATION) {

                SDL_ShowWindow(dsp.window);

                if (!emuReady) {
                    memset(&mem, 0, sizeof(mem));
                    memory_load_rom(&mem, "c8games/VERS", START_ADDRESS);
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
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();

        return 0;
    }


