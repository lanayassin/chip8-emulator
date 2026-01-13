
=========================================================================
Projet tutoré - Emulateur Chip-8
UE Programmation Impérative
Polytech Tours

Binôme : YASSIN Lana / COINTRE Pierre
=========================================================================

=========================================================================
0. Rappel du projet

L'objectif de ce projet était de créer un émulateur Chip-8 en C.
Ce projet nous a permis de découvrir le monde de l'émulation, et de nous
familiariser avec des projets complexes, qui nécessitent une organisation 
et un travail en équipe non négligeable.
=========================================================================

=========================================================================
1. Liste des objectifs et réalisations

- ROMs de test validé :
    logos : OK
    corax+ : OK
    flags : OK
    quirks : OK
    keypad : OK
    beep : OK

- ROMs de jeu : OK

- BONUS :
    - Le programme se ferme proprement via la croix de la fenêtre SDL.
    - Interface graphique avec menu et sélection des ROM.
    - Possibilité de relancer une ROM sans recompiler et/ou relancer le programme.
    - Documentation du code (commentaires + Doxygen)
=========================================================================

=========================================================================
2. Organisation du code

On a décidé de découper le code en plusieurs parties pour permettre une 
lecture efficace de ce dernier. 
On a décidé d'avoir un fichier différent pour la mémoire, le processeur,
les opcodes et l'interface, qui ont respectivement chacun un .c et .h.

Les différents modules sont :
-memory.c / memory.h :
    gestion de la mémoire Chip-8 et chargement des ROMs.

-processor.c / processor.h :
    implémentation du processeur, des registres, du fetch/decode/execute,
    gestion des timers et du clavier

-opcode.c / opcode.h :
    implémentation des instructions Chip-8 et table de dispatch

-interface.c / interface.h :
    menu principal et écran de sélection des ROMs.

-main.c :
    point d'entrée du programme, initialisation et boucle principale.

=========================================================================

=========================================================================
3. Compilation et linkage

Pour simplifier la compilation et le linkage, on a décidé de créer un 
makefile qui avec make, make run et make clean nous permet de compiler,
linker, lancer et nettoyer notre espace de travail.
=========================================================================

=========================================================================
4. Spécifications valgrind

Ayant eu des problèmes de fuite mémoire, on a décidé d'utiliser Valgrind,
sous les conseils de notre encadrant, afin de régler ces derniers.
=========================================================================
