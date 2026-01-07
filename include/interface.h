#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_W 600
#define WINDOW_H 400


enum Screen {
    SCREEN_MENU,
    SCREEN_ROM_SELECT,
    SCREEN_EMULATION
};

void handle_menu_events(enum Screen* screen, int* running, SDL_Event event);
void handle_select_events(enum Screen* screen, SDL_Event event,const char** selectedRom);
void render_menu(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY);
void render_select(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY);
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color);
void render_text_left(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color);




#endif