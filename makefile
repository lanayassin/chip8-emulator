# Compilateur C utilisé
CC      := gcc

# Standard C utilisé
CSTD    := -std=c11

# Options d'avertissements
WARN    := -Wall -Wextra -Wshadow -Wstrict-prototypes -Wconversion

# Optimisation du code
OPT     := -O2

# Répertoires d'inclusion des headers
INCDIRS := -Iinclude

# Flags de compilation pour SDL2 et SDL2_ttf
SDL2C   := $(shell pkg-config --cflags sdl2 SDL2_ttf)

# Flags de linkage pour SDL2 et SDL2_ttf
SDL2L   := $(shell pkg-config --libs sdl2 SDL2_ttf)

# Bibliothèque externe fournie
PROVIDED    := lib/x86_64/libprovided.a

# Fichiers sources
SRC := src/memory.c src/processor.c src/main.c src/opcode.c src/interface.c

# Fichiers objets générés à partie des sources
OBJ := $(SRC:.c=.o)

# Nom de l'exécutable final
BIN := chip8

.PHONY: all clean run valgrind

# Règle par défaut : compilation complète
all: $(BIN)

# Édition de liens pour générer l'exécutable
$(BIN): $(OBJ) $(PROVIDED)
	$(CC) $(CSTD) $(WARN) $(OPT) $(OBJ) -o $@ $(PROVIDED) $(SDL2L) -lm

# Compilation d'un fichier source en fichier objet
src/%.o: src/%.c
	$(CC) $(CSTD) $(WARN) $(OPT) $(INCDIRS) $(SDL2C) -c $< -o $@

# Suppression des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(BIN)

# Lance le programme
run: $(BIN)
	./$(BIN)

# Exécution avec Valgrind pour la détection de fuites mémoire
valgrind: $(BIN)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BIN)
