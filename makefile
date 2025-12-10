CC		:= gcc
CSTD	:= -std=c11
WARN	:= -Wall -Wextra -Wshadow -Wstrict-prototypes -Wconversion
OPT		:= -O2
INCDIRS	:= -Iinclude

SDL2C	:= $(shell pkg-config --cflags sdl2)
SDL2L	:= $(shell pkg-config --libs sdl2)

PROVIDED	:= lib/x86_64/libprovided.a

SRC	:= src/memory.c src/processor.c src/main.c
OBJ	:= $(SRC:.c=.o)
BIN	:= chip8

.PHONY: all clean run valgrind

all: $(BIN)

$(BIN): $(OBJ) $(PROVIDED)
	$(CC) $(CSTD) $(WARN) $(OPT) $(OBJ) -o $@ $(PROVIDED) $(SDL2L)

src/%.o: src/%.c
	$(CC) $(CSTD) $(WARN) $(OPT) $(INCDIRS) $(SDL2C) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

run: $(BIN)
	./$(BIN)

# --- Ajout ultra pratique ! ---
valgrind: $(BIN)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BIN)
