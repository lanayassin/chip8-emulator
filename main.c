#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    // 1. Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        // En cas d'échec de l'initialisation (très rare après l'installation)
        fprintf(stderr, "Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    // 2. Création d'une fenêtre simple (640x480 pixels)
    SDL_Window* window = SDL_CreateWindow("Test SDL2 CHIP-8",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640, 480,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL) {
        // En cas d'échec de la création de fenêtre
        fprintf(stderr, "Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    printf("SDL2 est fonctionnel ! La fenêtre va se fermer dans 3 secondes.\n");

    // 3. Pause pour que vous puissiez voir la fenêtre
    SDL_Delay(3000); // 3000 millisecondes = 3 secondes

    // 4. Nettoyage
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}