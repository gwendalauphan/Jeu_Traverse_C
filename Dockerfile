# Utiliser une image de base avec un environnement Linux et gcc installé
FROM ubuntu:latest

# Mettre à jour les paquets et installer les dépendances nécessaires
RUN apt-get update && apt-get install -y \
    zip \
    gcc \
    make \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev

# Définir une variable d'environnement pour XDG_RUNTIME_DIR
ENV XDG_RUNTIME_DIR=/tmp/runtime-root

# Assurer que le répertoire existe
RUN mkdir -p $XDG_RUNTIME_DIR && chmod 0700 $XDG_RUNTIME_DIR

# Copier le code source du projet dans le conteneur
COPY . /usr/src/traverse

# Définir le répertoire de travail
WORKDIR /usr/src/traverse

# Compiler le projet
RUN make all

# Commande pour exécuter l'application
CMD ["make", "run"]
