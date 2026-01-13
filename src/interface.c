/**
 * @file interface.c
 * @brief Gestion de l'interface graphique.
 * 
 * Ce fichier contient l'ensemble des fonctions liées à l'interface utilisateur.
 * Il permet à l'utilisateur de naviguer dans l'application et de lancer l'émulation de ROM Chip-8.
 * 
 */


#include "interface.h"

Uint32 ignoreMouseTime = 0;
int ignoreMouse = 0;

/**
 * @brief Gère les événements utilisateur sur l'écran du menu principal.
 * 
 * Détecte les clics sur les boutons du menu (Play et Quit).
 * Permet de changer d'écran ou de fermer l'application.
 * 
 * @param screen Écran actuellement affiché
 * @param running Indique si l'application doit continuer à s'exécuter
 * @param event Événement SDL à traiter
 */
void handle_menu_events(enum Screen* screen, int* running, SDL_Event event) {
    //Récupération de la position de la souris lors de l'événement
    int x = event.button.x;
    int y = event.button.y;

    //Définition des zones cliquables 
    SDL_Rect play = {180, 180, 240, 60};
    SDL_Rect quit = {220, 280, 160, 45};
    
    //Gestion des clics sur les boutons play et quit
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        //Changement d'écran
        if (x >= play.x && x <= play.x + play.w && y >= play.y && y <= play.y + play.h) {
            *screen = SCREEN_ROM_SELECT;

            //Mise en place d'un délai pour éviter les doubles clics involontaires
            ignoreMouse = 1;
            ignoreMouseTime = SDL_GetTicks() + 200;
        }
        //Arrêt et fermeture de la fenetre
        if (x >= quit.x && x <= quit.x + quit.w && y >= quit.y && y <= quit.y + quit.h) {
            *running = 0;
        }
    }
}

/**
 * @brief Gère les événements utilisateurs sur l'écran de sélection des ROM.
 * 
 * Permet de sélectionner une ROM à lancer ou de revenir au menu principal.
 * Un délai est utilisé pour éviter les clics involontaires lors du changement d'écran.
 * 
 * @param screen Ecran actuellement affiché
 * @param event Evénement SDL à traiter
 * @param selectedRom Chemin de la ROM sélectionnée
 */
void handle_select_events(enum Screen* screen,SDL_Event event, const char** selectedRom) {
    //Gestion d'un délai pour ignorer certains clics souris ( ces clics étaient trop rapides et selectionnaient directement une rom)
    if (ignoreMouse) {
        if (SDL_GetTicks() < ignoreMouseTime) {
            return; // on ignore ce clic
        } else {
            ignoreMouse = 0; // le délai est passé
        }
    }
    //Récupération de la position de la souris
    int x = event.button.x;
    int y = event.button.y;

    //Liste des chemins vers les différentes ROM
    const char* list_rom[] = {"romCh8/15PUZZLE", "romCh8/BLINKY", "romCh8/BLITZ", "romCh8/BRIX", "romCh8/CONNECT4", "romCh8/GUESS", "romCh8/HIDDEN", "romCh8/INVADERS", "romCh8/KALEID", "romCh8/MAZE", "romCh8/MERLIN", "romCh8/MISSILE", "romCh8/PONG", "romCh8/PONG2", "romCh8/PUZZLE", "romCh8/SYZYGY", "romCh8/TANK", "romCh8/TETRIS", "romCh8/TICTAC", "romCh8/UFO", "romCh8/VBRIX", "romCh8/VERS", "romCh8/WIPEOFF", "romCh8/1-chip8-logo.ch8", "romCh8/2-ibm-logo.ch8", "romCh8/3-corax+.ch8", "romCh8/4-flags.ch8", "romCh8/5-quirks.ch8", "romCh8/6-keypad.ch8", "romCh8/7-beep.ch8"};

    //Définition des paramètres permettant l'affichage correct de la liste des roms
    int rom_count = sizeof(list_rom) / sizeof(list_rom[0]);

    int columns = 3;
    int columnWidth = 180;
    int startX = 30;
    int startY = 90;
    int lineHeight = 20;
    int maxLines = (WINDOW_H - startY - 80) / lineHeight;

    //Définition de la zone du bouton retour
    SDL_Rect retour = {180, 330, 240, 50};
    
    //Gestion de la sélection d'une ROM
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        for (int i = 0; i < rom_count; i++) {
            int col = i / maxLines;
            int row = i % maxLines;
            if (col >= columns) {
                break;
            }

            SDL_Rect rect = {startX + col * columnWidth, startY + row * lineHeight, columnWidth, lineHeight};
            //Lancement de l'émulation
            if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) {
                *selectedRom = list_rom[i];
                *screen = SCREEN_EMULATION;
                return;
            }
        }

        //Retour au menu principal
        if (x >= retour.x && x <= retour.x + retour.w && y >= retour.y && y <= retour.y + retour.h) {
            *screen = SCREEN_MENU;
        }
    }

}

/**
 * @brief Affiche le menu principal.
 * 
 * Dessine le fond, les boutons interactifs et le titre.
 * Gère l'effet de hover sur les boutons.
 * 
 * @param r Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour les textes des boutons.
 * @param mouseX Position X de la souris
 * @param mouseY Position Y de la souris
 */
void render_menu(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY) {
    //Couleur de fond de la fenetre
    SDL_SetRenderDrawColor(r, 255, 200, 255, 255);
    SDL_RenderClear(r);

    //Définition des boutons
    SDL_Rect play = {180, 180, 240, 60};
    SDL_Rect quit = {220, 280, 160, 45};

    //Définition des bordures
    int border_thickness = 4;
    SDL_Color borderColor = {255, 38, 227, 255};

    //Définition des couleurs
    SDL_Color playFill, playText;
    SDL_Color quitFill, quitText;

    //Gestion du hover du bouton play
    if (mouseX >= play.x && mouseX <= play.x + play.w &&
        mouseY >= play.y && mouseY <= play.y + play.h) {
        playFill = (SDL_Color){255, 38, 227, 255};    
        playText = (SDL_Color){255, 200, 255, 255};   
    } else {
        playFill = (SDL_Color){255, 200, 255, 255};   
        playText = (SDL_Color){255, 38, 227, 255};   
    }

    //Gestion du hover du bouton quit
    if (mouseX >= quit.x && mouseX <= quit.x + quit.w &&
        mouseY >= quit.y && mouseY <= quit.y + quit.h) {
        quitFill = (SDL_Color){255, 38, 227, 255};
        quitText = (SDL_Color){255, 200, 255, 255};
    } else {
        quitFill = (SDL_Color){255, 200, 255, 255};
        quitText = (SDL_Color){255, 38, 227, 255};
    }

    //Génération du bouton play
    SDL_SetRenderDrawColor(r, playFill.r, playFill.g, playFill.b, playFill.a);
    SDL_RenderFillRect(r, &play);
    SDL_SetRenderDrawColor(r, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i < border_thickness; i++) {
        SDL_Rect b = {play.x - i, play.y - i, play.w + 2*i, play.h + 2*i};
        SDL_RenderDrawRect(r, &b);
    }

    //Génération du bouton quit
    SDL_SetRenderDrawColor(r, quitFill.r, quitFill.g, quitFill.b, quitFill.a);
    SDL_RenderFillRect(r, &quit);
    SDL_SetRenderDrawColor(r, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i < border_thickness; i++) {
        SDL_Rect b = {quit.x - i, quit.y - i, quit.w + 2*i, quit.h + 2*i};
        SDL_RenderDrawRect(r, &b);
    }

    //Génération des textes des boutons
    render_text(r, font, "PLAY", play, playText);
    render_text(r, font, "QUIT", quit, quitText);

    //Génération du titre
    SDL_Rect titleRect = {0, 50, 600, 50};
    TTF_Font* titleFont = TTF_OpenFont("fonts/Audiowide-Regular.ttf", 48);
    if (titleFont) {
        render_text(r, titleFont, "PolyChip8", titleRect, (SDL_Color){255,38,227,255});
        TTF_CloseFont(titleFont);
    }

    SDL_RenderPresent(r);
}   

/**
 * @brief Affiche l'écran de sélection des ROM.
 * 
 * Affiche la liste des ROM disponibles.
 * Gère le hover des éléments et affiche le bouton de retour.
 * 
 * @param r Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour l'affichage des ROM.
 * @param mouseX Position X de la souris.
 * @param mouseY Position Y de la souris.
 */
void render_select(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY) {
    //Couleur de fond de la fenetre
    SDL_SetRenderDrawColor(r, 255, 200, 255, 255);
    SDL_RenderClear(r);

    //Définition des couleurs
    SDL_Color normalColor = {255, 38, 227, 255};
    SDL_Color hoverColor  = {150, 0, 150, 255};

    //Liste des noms des différentes ROM (ces noms seront affichés à l'écran)
    const char* list_rom [] = {"15PUZZLE","BLINKY", "BLITZ", "BRIX", "CONNECT4", "GUESS", "HIDDEN", "INVADERS", "KALEID", "MAZE", "MERLIN", "MISSILE", "PONG", "PONG2", "PUZZLE", "SYZYGY", "TANK", "TETRIS", "TICTAC", "UFO", "VBRIX", "VERS", "WIPEOFF", "1-chip8-logo.ch8", "2-ibm-logo.ch8", "3-corax+.ch8", "4-flags.ch8", "5-quirks.ch8", "6-keypad.ch8", "7-beep.ch8" };

    //Défintiion des paramètres permettant l'affichage correct de la liste des roms
    int rom_count = sizeof(list_rom) / sizeof(list_rom[0]);

    int columns = 3;
    int columnWidth = 180;
    int startX = 30;
    int startY = 90;
    int lineHeight = 20;

    int maxLines = (WINDOW_H - startY - 80) / lineHeight;

    //Affichage de la liste des roms
    for (int i = 0; i < rom_count; i++) {
        int col = i / maxLines;
        int row = i % maxLines;

        if (col >= columns) break;

        SDL_Rect textRect = {startX + col * columnWidth, startY + row * lineHeight, columnWidth, lineHeight};

        SDL_Color color = normalColor;

        //Gestion du hover des noms des roms
        if (mouseX >= textRect.x && mouseX <= textRect.x + textRect.w && mouseY >= textRect.y && mouseY <= textRect.y + textRect.h) {
            color = hoverColor;
        }

        render_text_left(r, font, list_rom[i], textRect, color);
    }

    //Définition du boutons retour
    SDL_Rect retour = {180, 330, 240, 50};

    //Définition des bordures
    int border_thickness = 4;
    SDL_Color borderColor = {255, 38, 227, 255};

    //Couleur pour le bouton retour
    SDL_Color retourFill, retourText;

    //Gestion du hover du bouton retour
    if (mouseX >= retour.x && mouseX <= retour.x + retour.w && mouseY >= retour.y && mouseY <= retour.y + retour.h) {
        retourFill = (SDL_Color){255, 38, 227, 255};
        retourText = (SDL_Color){255, 200, 255, 255};
    } else {
        retourFill = (SDL_Color){255, 200, 255, 255};
        retourText = (SDL_Color){255, 38, 227, 255};
    }

    //Affichage du bouton retour
    SDL_SetRenderDrawColor(r, retourFill.r, retourFill.g, retourFill.b, retourFill.a);
    SDL_RenderFillRect(r, &retour);
    SDL_SetRenderDrawColor(r, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i < border_thickness; i++) {
        SDL_Rect b = {retour.x - i, retour.y - i, retour.w + 2*i, retour.h + 2*i};
        SDL_RenderDrawRect(r, &b);
    }

    //Affichage du texte du bouton retour
    TTF_Font* font_text = TTF_OpenFont("fonts/Comfortaa-VariableFont_wght.ttf", 32);
    render_text(r, font_text, "RETOUR", retour, retourText);

    //Génération du titre
    SDL_Rect titleRect = {0, 20, 600, 50};
    TTF_Font* titleFont = TTF_OpenFont("fonts/Audiowide-Regular.ttf", 48);
    if (titleFont) {
        render_text(r, titleFont, "PolyChip8", titleRect, (SDL_Color){255,38,227,255});
        TTF_CloseFont(titleFont);
    }   

    SDL_RenderPresent(r);
}

/**
 * @brief Affiche un texte centré dans un rectangle
 * 
 * Crée une texture à partir du texte et l'affiche centré horizontalement et verticalement.
 * 
 * @param renderer Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour le texte.
 * @param text Texte à affciher.
 * @param rect Rectangle de référence.
 * @param color Couleur du texte.
 */
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color) {
    //Création de la surface et de la texture à partir du texte
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    //Récupération de la taille du texte généré
    int textW = 0, textH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);

    //Calcul de la position pour centrer le texte dans le rectangle
    SDL_Rect dst = {rect.x + (rect.w - textW)/2, rect.y + (rect.h - textH)/2, textW, textH};

    //Affichage du texte à l'écran
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    //Libération de la mémoire utilisée
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/**
 * @brief Affiche un texte aligné à gauche dans un rectangle.
 * 
 * Crée une texture à partir du texte et l'affiche alignée à gauche et centrée verticalement.
 * 
 *  @param renderer Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour le texte.
 * @param text Texte à afficher.
 * @param rect Rectangle de référence.
 * @param color Couleur du texte.
*/
void render_text_left(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color) {
    //Création de la surface et de la texture à partir du texte
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    //Récupération de la taille du texte généré
    int textW, textH;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);

    //Positionnement du texte aligné à gauche dans le rectangle
    SDL_Rect dst = {rect.x + 4, rect.y + (rect.h - textH) / 2, textW, textH};

    //Affichage du texte à l'écran
    SDL_RenderCopy(renderer, texture, NULL, &dst);

    //Libération de la mémoire utilisée
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

