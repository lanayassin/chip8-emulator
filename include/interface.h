/** 
 * @file interface.h
 * @brief Déclarations des fonctions et types liés à l'interface graphique.
 * 
 * Ce fichier contient les définitions des constantes, types et prototypes nécessaires à la gestion de l'interface utilisateur.
 * Il est utilisé par les différents modules de l'application.
 */
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/**
 * @def WINDOW_W
 * @brief Largeur de la fenêtre de l'application.
 */
#define WINDOW_W 600

/**
 * @def WINDOW_H
 * @brief Hauteur de la fenêtre de l'application.
 */
#define WINDOW_H 400

/**
 * @enum Screen
 * @brief Représente les différents écrans de l'application.
 * 
 * Cette énumération permet de gérer les transitions entre le menu principal, la sélection des ROM et l'écran d'émulation.
 */
enum Screen {
    SCREEN_MENU, /**< Ecran du menu principal*/
    SCREEN_ROM_SELECT, /**< Ecran de sélection des ROM*/
    SCREEN_EMULATION /**< Ecran d'émulation*/
};

/**
 * @brief Gère les événements utilisateur sur l'écran du menu principal.
 * 
 * @param screen Écran actuellement affiché
 * @param running Indique si l'application doit continuer à s'exécuter
 * @param event Événement SDL à traiter
 */
void handle_menu_events(enum Screen* screen, int* running, SDL_Event event);

/**
 * @brief Gère les événements utilisateurs sur l'écran de sélection des ROM.
 * 
 * @param screen Ecran actuellement affiché
 * @param event Evénement SDL à traiter
 * @param selectedRom Chemin de la ROM sélectionnée
 */
void handle_select_events(enum Screen* screen, SDL_Event event,const char** selectedRom);

/**
 * @brief Affiche le menu principal.
 * 
 * @param r Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour les textes des boutons.
 * @param mouseX Position X de la souris
 * @param mouseY Position Y de la souris
 */
void render_menu(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY);

/**
 * @brief Affiche l'écran de sélection des ROM.
 * 
 * @param r Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour l'affichage des ROM.
 * @param mouseX Position X de la souris.
 * @param mouseY Position Y de la souris.
 */
void render_select(SDL_Renderer* r, TTF_Font* font, int mouseX, int mouseY);

/**
 * @brief Affiche un texte centré dans un rectangle.
 * 
 * @param renderer Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour le texte.
 * @param text Texte à afficher.
 * @param rect Rectangle de référence.
 * @param color Couleur du texte.
 */
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color);

/**
 * @brief Affiche un texte aligné à gauche dans un rectangle.
 * 
 * @param renderer Renderer SDL utilisé pour l'affichage.
 * @param font Police utilisée pour le texte.
 * @param text Texte à afficher.
 * @param rect Rectangle de référence.
 * @param color Couleur du texte.
 */
void render_text_left(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color);

#endif