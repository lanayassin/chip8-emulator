#include "interface.h"


Uint32 ignoreMouseTime = 0;
int ignoreMouse = 0;



void handle_menu_events(enum Screen* screen, int* running, SDL_Event event) {

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x = event.button.x;
        int y = event.button.y;
        SDL_Rect play = {180, 180, 240, 60};
        SDL_Rect quit = {220, 280, 160, 45};


        if (x >= play.x && x <= play.x + play.w &&
            y >= play.y && y <= play.y + play.h) {
            *screen = SCREEN_ROM_SELECT;

            ignoreMouse = 1;
            ignoreMouseTime = SDL_GetTicks() + 200;
        }

        if (x >= quit.x && x <= quit.x + quit.w &&
            y >= quit.y && y <= quit.y + quit.h) {
            *running = 0;
        }
    }
}

void handle_select_events(enum Screen* screen,SDL_Event event, const char** selectedRom)
{
    if (ignoreMouse) {
        if (SDL_GetTicks() < ignoreMouseTime) {
            return; // on ignore ce clic
        } else {
            ignoreMouse = 0; // le délai est passé
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT) {

        int x = event.button.x;
        int y = event.button.y;

        const char* list_rom[] = {
            "romCh8/15PUZZLE","romCh8/BLINKY","romCh8/BLITZ","romCh8/BRIX",
            "romCh8/CONNECT4","romCh8/GUESS","romCh8/HIDDEN","romCh8/INVADERS",
            "romCh8/KALEID","romCh8/MAZE","romCh8/MERLIN","romCh8/MISSILE",
            "romCh8/PONG","romCh8/PONG2","romCh8/PUZZLE","romCh8/SYZYGY",
            "romCh8/TANK","romCh8/TETRIS","romCh8/TICTAC","romCh8/UFO",
            "romCh8/VBRIX","romCh8/VERS","romCh8/WIPEOFF", "romCh8/1-chip8-logo.ch8",
             "romCh8/2-ibm-logo.ch8", "romCh8/3-corax+.ch8", "romCh8/4-flags.ch8", "romCh8/5-quirks.ch8",
              "romCh8/6-keypad.ch8", "romCh8/7-beep.ch8"
        };

        int rom_count = sizeof(list_rom) / sizeof(list_rom[0]);

        int columns = 3;
        int columnWidth = 180;
        int startX = 30;
        int startY = 90;
        int lineHeight = 20;
        int maxLines = (WINDOW_H - startY - 80) / lineHeight;

        for (int i = 0; i < rom_count; i++) {
            int col = i / maxLines;
            int row = i % maxLines;
            if (col >= columns) break;

            SDL_Rect rect = {
                startX + col * columnWidth,
                startY + row * lineHeight,
                columnWidth,
                lineHeight
            };

            if (x >= rect.x && x <= rect.x + rect.w &&
                y >= rect.y && y <= rect.y + rect.h) {

                *selectedRom = list_rom[i];
                *screen = SCREEN_EMULATION;
                return;
            }
        }

        SDL_Rect retour = {180, 330, 240, 50};
        if (x >= retour.x && x <= retour.x + retour.w &&
            y >= retour.y && y <= retour.y + retour.h) {
            *screen = SCREEN_MENU;
        }
    }
}


void render_menu(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY) {
    // Fond de la fenêtre
    SDL_SetRenderDrawColor(r, 255, 200, 255, 255);
    SDL_RenderClear(r);

    // Définition des boutons
    SDL_Rect play = {180, 180, 240, 60};
    SDL_Rect quit = {220, 280, 160, 45};

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
    TTF_Font* titleFont = TTF_OpenFont("fonts/Audiowide-Regular.ttf", 48);
    if (titleFont) {
        render_text(r, titleFont, "PolyChip8", titleRect, (SDL_Color){255,38,227,255});
        TTF_CloseFont(titleFont);
    }

    SDL_RenderPresent(r);
}

void render_select(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY) {

    // Fond de la fenêtre
    SDL_SetRenderDrawColor(r, 255, 200, 255, 255);
    SDL_RenderClear(r);

    SDL_Color normalColor = {255, 38, 227, 255};
    SDL_Color hoverColor  = {150, 0, 150, 255}; // plus foncé

    const char* list_rom [] = {"15PUZZLE","BLINKY", "BLITZ", "BRIX", "CONNECT4", "GUESS", "HIDDEN", "INVADERS", "KALEID", "MAZE", "MERLIN", "MISSILE", "PONG", "PONG2", "PUZZLE", "SYZYGY", "TANK", "TETRIS", "TICTAC", "UFO", "VBRIX", "VERS", "WIPEOFF", "1-chip8-logo.ch8", "2-ibm-logo.ch8", "3-corax+.ch8", "4-flags.ch8", "5-quirks.ch8", "6-keypad.ch8", "7-beep.ch8" };
    
    int rom_count = sizeof(list_rom) / sizeof(list_rom[0]);

    int columns = 3;
    int columnWidth = 180;
    int startX = 30;
    int startY = 90;
    int lineHeight = 20;

    int maxLines = (WINDOW_H - startY - 80) / lineHeight;


    for (int i = 0; i < rom_count; i++) {
        int col = i / maxLines;
        int row = i % maxLines;

        if (col >= columns) break;

        SDL_Rect textRect = {
            startX + col * columnWidth,
            startY + row * lineHeight,
            columnWidth,
            lineHeight
        };

        SDL_Color color = normalColor;

        // HOVER
        if (mouseX >= textRect.x && mouseX <= textRect.x + textRect.w &&
            mouseY >= textRect.y && mouseY <= textRect.y + textRect.h) {
            color = hoverColor;
        }

        render_text_left(r, font, list_rom[i], textRect, color);
    }



    // Définition des boutons
    SDL_Rect retour = {180, 330, 240, 50};
    int border_thickness = 4;
    SDL_Color borderColor = {255, 38, 227, 255}; // violet

    // Couleurs pour chaque bouton
    SDL_Color retourFill, retourText;


    // Hover retour
    if (mouseX >= retour.x && mouseX <= retour.x + retour.w &&
        mouseY >= retour.y && mouseY <= retour.y + retour.h) {
        retourFill = (SDL_Color){255, 38, 227, 255};
        retourText = (SDL_Color){255, 200, 255, 255};
    } else {
        retourFill = (SDL_Color){255, 200, 255, 255};
        retourText = (SDL_Color){255, 38, 227, 255};
    }


    // Dessin retour
    SDL_SetRenderDrawColor(r, retourFill.r, retourFill.g, retourFill.b, retourFill.a);
    SDL_RenderFillRect(r, &retour);
    SDL_SetRenderDrawColor(r, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i < border_thickness; i++) {
        SDL_Rect b = {retour.x - i, retour.y - i, retour.w + 2*i, retour.h + 2*i};
        SDL_RenderDrawRect(r, &b);
    }

    // Texte des boutons
    TTF_Font* font_text = TTF_OpenFont("fonts/Comfortaa-VariableFont_wght.ttf", 32);
    render_text(r, font_text, "RETOUR", retour, retourText);
    // Titre
    SDL_Rect titleRect = {0, 20, 600, 50};
    TTF_Font* titleFont = TTF_OpenFont("fonts/Audiowide-Regular.ttf", 48);
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

void render_text_left(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW, textH;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);

    SDL_Rect dst = {
        rect.x + 4,                      // marge gauche
        rect.y + (rect.h - textH) / 2,   // centré verticalement
        textW,
        textH
    };

    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

