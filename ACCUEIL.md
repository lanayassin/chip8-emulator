
\mainpage Projet tutoré - Emulateur Chip-8

\brief Documentation du projet tutoré de l'UE Programmation Impérative à Polytech Tours.

\section introduction Introduction

**Binôme :** YASSIN Lana / COINTRE Pierre

Ce projet avait pour objectif de créer un émulateur Chip-8 en C.  
Il nous a permis de découvrir le monde de l'émulation et de nous familiariser avec des projets complexes nécessitant organisation et travail en équipe.

\section objectifs Objectifs et réalisations

\subsection roms_test ROMs de test validées
- logos : OK  
- corax+ : OK  
- flags : OK  
- quirks : OK  
- keypad : OK  
- beep : OK  

\subsection roms_jeu ROMs de jeu
- Tous les jeux testés : OK  

\subsection bonus Bonus réalisés
- Le programme se ferme proprement via la croix de la fenêtre SDL.  
- Interface graphique avec menu et sélection des ROMs.  
- Possibilité de relancer une ROM sans recompiler et/ou relancer le programme.  
- Documentation du code (commentaires + Doxygen).

\section organisation Organisation du code

Le code a été découpé en plusieurs modules pour faciliter la lecture et la maintenance :

\subsection memory memory.c / memory.h
Gestion de la mémoire Chip-8 et chargement des ROMs.

\subsection processor processor.c / processor.h
Implémentation du processeur, des registres, du fetch/decode/execute, gestion des timers et du clavier.

\subsection opcode opcode.c / opcode.h
Implémentation des instructions Chip-8 et table de dispatch.

\subsection interface interface.c / interface.h
Menu principal et écran de sélection des ROMs.

\subsection main main.c
Point d'entrée du programme, initialisation et boucle principale.

\section compilation Compilation et linkage

Pour simplifier la compilation et le linkage, un **Makefile** a été créé.  
Commandes disponibles :  
\code{.bash}
make        # Compiler
make run    # Lancer le programme
make clean  # Nettoyer l'espace de travail
\endcode

\section valgrind Spécifications Valgrind

Afin de résoudre les problèmes de fuite mémoire, Valgrind a été utilisé pour analyser et corriger le code.


