# Projet Traverse

Vous trouverez ici quelques informations utiles

## Installation
Pour installer le projet , il suffit de copier-coller le dossier du projet. Le projet n'est utilisable que sur Windows.
Afin de lancer le projet, il faut installer les librairies SDL2, SDL2_TTF et SDL2_Image.

## Compiler
Pour compiler et exécuter le programme, il suffit d'ouvrir un termnal dans le dossier du projet et d'exécuter la commande **make** ou **make all**.

Pour seulement compiler, il faut exécuter **make JeuTraverse**.

## Utiliser l'application
Pour lancer l'application, il suffit de lancer JeuTraverse.exe. Une fenêtre de dialogue apparait nous énonçant les règles du jeu.
Ensuite une autre fenêtre se lance demandant combien de personne (humain) vont jouer.
Enfin une dernière fenêtre demande le nombre de bots (ordinateurs) qui vont jouer.
Une fois les informations remplies, la fenêtre principale se lance et il suffit de cliquer avec la souris pour interagir avec le jeu.
Lors du clique sur une forme, les déplacements possibles sont proposés au joueur et il a juste a cliquer sur celui de son choix pour que son pion se déplace là où il a cliqué.
En pleine partie, à n'importe quel moment le joueur peut sauvegarder sa partie ou récupérer une ancienne sauvegarde avec les boutons situés en bas à droite de l'application.
Â la fin de la partie, il est proposé à l'utilisateur si il veut recommencer une partie ou si il veut quitter.

## Tests
Il existe deux tests, pour les lancer, il faut lancer l'application et appuyer sur le bouton de chargement de partie.

Les deux tests sont :

  - **Fin_de_jeu** dans lequel le joueur 1 est sur le point de gagner
  - **Perdus** dans lequel le joueur 2 va perdre

Pour tester l'égalité et les sauts, je vous invite à lancer l'application et à commencer à bouger les jetons.


## Problème connus
Il est possible que lors d'une partie contre un bot le jeu cesse de fonctionner ou plante, cela est surement dût aux inconsistances du language utilisé car côté code, il n'y a pas d'erreur.
