CC = gcc
CFLAGS = -Wall -D_REENTRANT $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_image)
LFLAGS = $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_image) -lm
SRC_DIR = src
OBJ_DIR = obj
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
EXEC = JeuTraverse

# Configuration des bibliothèques et de la commande de nettoyage selon l'OS
ifeq ($(OS),Windows_NT)
    LIBS = -lm -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
    CLEANUP = del $(subst /,\,$(OBJECTS)) $(EXEC).exe
else
    LIBS = $(shell pkg-config --libs --cflags sdl2 SDL2_ttf SDL2_image)
    CLEANUP = rm -f $(OBJECTS) $(EXEC)
endif

# Cibles phony pour éviter des conflits de noms
.PHONY: all clean run

# Cible principale pour la compilation
all: $(EXEC)
	@echo "Compilation complete!"

# Cible pour exécuter l'exécutable
run: all
	./$(EXEC)
	make clean

# Lier les fichiers objets pour créer l'exécutable
$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

# Compiler les fichiers source en fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# Nettoyer les fichiers objets et l'exécutable
clean:
	$(CLEANUP)
