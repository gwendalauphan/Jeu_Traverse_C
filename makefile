cc = gcc -Wall
FILES = obj/main.o obj/functions.o obj/graphics.o obj/minmax.o obj/save.o #liste des fichiers sources avec .o
OPT = -lm -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

all: JeuTraverse #commande pour compiler et lancer le jeu
	JeuTraverse.exe
	
JeuTraverse : $(FILES) #commande pour compiler le jeu
	$(cc) $^ -o $@ $(OPT)

obj/%o: src/%c #commande de création des .o avec les .c
	$(cc) -c $^ -o $@ $(OPT) 

clean: #commande pour retirer tous les fichiers objects
	rm obj/*.o