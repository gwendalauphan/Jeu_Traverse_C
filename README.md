# Projet Traverse

Vous trouverez ici quelques informations utiles

## Prérequis

### Installer GCC

#### Sous Linux
GCC peut être installé via le gestionnaire de paquets. Ouvrez un terminal et exécutez :
```bash
sudo apt-get update
sudo apt-get install gcc
```

#### Sous Windows
Pour installer GCC sous Windows, vous pouvez utiliser MinGW. Suivez ces étapes :
1. Téléchargez MinGW depuis [le site officiel](http://mingw.org/).
2. Installez MinGW et lors de l'installation, assurez-vous de sélectionner `mingw32-base-bin` et `mingw32-gcc-g++-bin`.
3. Ajoutez le chemin du répertoire `bin` de MinGW à la variable d'environnement PATH.

### Installer les bibliothèques SDL2

#### Sous Linux
Exécutez les commandes suivantes pour installer SDL2 et ses extensions :
```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

#### Sous Windows
Installez les bibliothèques SDL2 en téléchargeant les fichiers de développement depuis les sites officiels et en configurant les chemins dans votre IDE ou environnement de développement.

## Compilation

Pour compiler le jeu, ouvrez un terminal ou une invite de commande dans le dossier du projet et tapez :
```bash
make all
```

## Utilisation

Lancez le jeu avec l'exécutable généré (`./2048` sous Linux ou double-cliquez sur `2048.exe` sous Windows). Au lancement, sélectionnez le mode de jeu désiré.



## Avec Docker

#### Prérequis

Installer Docker :
- Linux : Suivre les instructions sur [docker.com](https://docs.docker.com/engine/install/).

#### Construire l'image Docker

Dans le répertoire du projet, exécutez :
```bash
docker build -t jeutraverse .
```

#### Exécuter le conteneur Docker

Autoriser l'accès à X11 :
```bash
xhost +local:docker
```

Lancer le conteneur :
```bash
docker run -it --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --device /dev/dri:/dev/dri jeutraverse
```

Révoquer l'accès X11 après utilisation :
```bash
xhost -local:docker
``` 

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
