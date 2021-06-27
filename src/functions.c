#include <stdio.h>
#include <math.h>
#include "../headers/types.h"

/*Fichier où sont regroupées toutes les fonctions nécésaires au bon fonctionnement du programme*/

//data des formes
Forme carre = {.nb_deplacement = 4, .deplacement = {{1, 0}, {-1, 0}, {0, -1}, {0, 1}}, .id = 0};
Forme triangle = {.nb_deplacement = 3, .deplacement = {{1, 1}, {1, -1}, {-1, 0}}, .id = 1};
Forme losange = {.nb_deplacement = 4, .deplacement = {{1, 1}, {-1, -1}, {1, -1}, {-1, 1}}, .id = 2};
Forme cercle = {.nb_deplacement = 8, .deplacement = {{1, 0}, {-1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}}, .id = 3};

//couleurs des pions
//SDL_Color black = {0, 0, 0, 255};
SDL_Color violet = {223, 115, 255, 255};
SDL_Color yellow = {252, 194, 5, 255};
SDL_Color red = {255, 50, 50, 255};
SDL_Color green = {50, 255, 50, 255};

/*Fonction pour appliquer une rotation d'un point par un anglme autours du centre du repère*/
SDL_Point applyRotation(float rotation, SDL_Point vect){
    SDL_Point res;
    float a = rotation*3.141592653/2.0; //à chaque fois, on effectur une rotation de kpi/2 avec comme seule variable k (rotation dans le code)
    res.x = round(cos(a)*vect.x - sin(a)*vect.y);
    res.y = round(sin(a)*vect.x + cos(a)*vect.y);
    return res;
}

/*
Fonction pour initialiser les joueurs et leurs pions.
Les joeurs sont en face à face à deux, en haut et en bas et en cercle lorsqu'ils sont 3 ou plus
Chaque joueur a 8 pions, 2 par type
*/
void reset(int PlayerCount, Player Playerslist[4]){
    //attribution des rotations aux joueurs
    Playerslist[0].rotation = 1.0;
    Playerslist[1].rotation = -1.0;
    if (PlayerCount >= 3){
        Playerslist[2].rotation = 2.0;
        if (PlayerCount == 4) {
            Playerslist[3].rotation = 0.0;
        }
    }

    //attribution des propriétées aux pions
    for (int i = 0; i < PlayerCount; i++){ //boucle dans tous les joueurs
        //pour les triangles
        Playerslist[i].piece[0] = Playerslist[i].piece[7] = carre;
        Playerslist[i].piece[1] = Playerslist[i].piece[6] = triangle;
        Playerslist[i].piece[2] = Playerslist[i].piece[5] = losange;
        Playerslist[i].piece[3] = Playerslist[i].piece[4] = cercle;

        for (int j = 0; j < 8; j++){
            //calcul des déplacements possible
            for (int k = 0; k < Playerslist[i].piece[j].nb_deplacement; k++){
                SDL_Point rotated;
                rotated = applyRotation(Playerslist[i].rotation, Playerslist[i].piece[j].deplacement[k]);
                Playerslist[i].piece[j].deplacement[k] = rotated;
            }

            //initialisation de l'historique
            for (int n = 0; n < 100; n++){
                Playerslist[i].piece[j].history[n] = 0;
            }
        }
    }
  
    //initisalisation des positions initiales des pions à partir d'un fichier texte
    FILE* fichier = fopen("res/positions.txt", "r");
    int current = 0, i=0, j=0;
    int index[4] = {0, 0, 0, 0}; //liste des index pour les positions de leurs pions
    do
    {
        current = fgetc(fichier);
        if (current == '1' || current == '2' || current == '3' || current == '4' || current == '0'){
            switch (current){
                case '1':
                    Playerslist[0].piece[index[0]].position.x = i;
                    Playerslist[0].piece[index[0]].position.y = j;
                    Playerslist[0].piece[index[0]].history[j*10 + i] ++;
                    index[0] ++;
                    break;
                case '3':
                    Playerslist[1].piece[index[2]].position.x = i;
                    Playerslist[1].piece[index[2]].position.y = j;
                    Playerslist[1].piece[index[2]].history[j*10 + i] ++;
                    index[2] ++;
                    break;
                case '2':
                    if (PlayerCount != 2){
                        Playerslist[2].piece[index[1]].position.x = i;
                        Playerslist[2].piece[index[1]].position.y = j;
                        Playerslist[2].piece[index[1]].history[j*10 + i] ++;
                        index[1] ++;
                    }
                    break;
                case '4':
                    if (PlayerCount == 4){
                        Playerslist[3].piece[index[3]].position.x = i;
                        Playerslist[3].piece[index[3]].position.y = j;
                        Playerslist[3].piece[index[3]].history[j*10 + i] ++;
                        index[3] ++;
                    }
                    break;
            }
            //actualisation de l'emplacement dans le fichier
            i = (i+1)%10;
            if (i == 0) j++;
        }
    } while (current != EOF);
    fclose(fichier);
}

/*Focntion pour vérifier si une case est libre (playerlist, playerCount, point)*/
int isFree(Player * PlayersList, int PlayerCount, SDL_Point point){
    int free = 1;
    if (point.x < 0 || point.x > 9 || point.y < 0 || point.y > 9) free = 0; //vérification de la validitée du point à vérifier
    //boucle dans tous les joueurs pour savoir si la case est libre
    for (int i = 0; i < PlayerCount; i ++){
        for (int j = 0; j < 8; j++){
            if (point.x == PlayersList[i].piece[j].position.x && point.y == PlayersList[i].piece[j].position.y) free = 0;
        }
    }
    return free;
}

/*Fonction récursive pour récupérer la liste des mouvements possibles par un pion (pion, playerlist, playercount, playerid, point, array, length, depth)*/
void validMove(Forme pion, Player * PLayersList, int playerCount, int playerId, SDL_Point point, SDL_Point array[30], int * index, int depth){
    int not = 0;
    for (int i = 0; i < *index-1; i++){
        if (array[i].x == point.x && array[i].y == point.y) not = 1;
    }
    if (depth > 12 || (pion.position.x == point.x && pion.position.y == point.y && depth > 0) || not) return; // si une des conditions (retour sur le point, excès en profondeur)
    
    int x, y;
    for (int i = 0; i < pion.nb_deplacement; i ++){ //sinon pour chaque déplacement
        x = point.x + pion.deplacement[i].x;
        y = point.y + pion.deplacement[i].y;
        if (x < 10 && x >= 0 && y < 10 && y >= 0){
            SDL_Point tmp = {x, y};
            if (!isFree(PLayersList, playerCount, tmp)){ //vérification si les déplacement possibles sont libres
                SDL_Point toTest;
                toTest.x = point.x + pion.deplacement[i].x*2;
                toTest.y = point.y + pion.deplacement[i].y*2;
                if (isFree(PLayersList, playerCount, toTest)){ //vrification si un jump est possible si la case adjascente est opccupée
                    array[*index] = toTest;
                    *index += 1;
                    validMove(pion, PLayersList, playerCount, playerId, toTest, array, index, depth + 1);
                }
            }
            else if (depth == 0){
                array[*index] = tmp;
                *index += 1;
            }
        }
    }

    //à la fin, les mouvements qui finissent sur les bordures sont éliminés
    if (depth == 0){
        int lenTmp = 0;
        SDL_Point tesTmp[30];

        for (int i = 0; i < *index; i++){
            if (array[i].x > 0 && array[i].x < 9 && array[i].y > 0 && array[i].y < 9){
                tesTmp[lenTmp++] = array[i];
            }
            if ((playerId == 0 && (array[i].y == 9 || array[i].y == 0) && array[i].x < 9 && array[i].x > 0) || (playerId == 1 && (array[i].y == 0 || array[i].y == 0) && array[i].x < 9 && array[i].x > 0)\
             || (playerId == 2 && (array[i].x == 9 || array[i].x == 0) && array[i].x < 9 && array[i].x > 0) || (playerId == 3 && (array[i].x == 0 || array[i].x == 9) && array[i].x < 9 && array[i].x > 0)){
                tesTmp[lenTmp++] = array[i];
            }
        }

        for (int i = 0; i < lenTmp; i++){
            array[i] = tesTmp[i];
        }
        *index = lenTmp;
    }
}

/*Fonction pour récupérer le pion situé à un certain endroit*/
Forme *pawnAt(Player * Player, int x, int y, Forme NotPawn){
    Forme * atDestination = &NotPawn; //par défaut on renvoie un non-point
    if (x < 0 || x > 9 || y < 0 || y > 9) return atDestination; //vérification de la validité des cvoordonées
        for (int j = 0; j < 8; j++){ //parcours les pions et renvoiis du résultat
            if (x == Player->piece[j].position.x && y == Player->piece[j].position.y) atDestination = &(Player->piece[j]);
        }
    return atDestination;
}

/*Fonction qui renvoie l'état d'un joueur*/
int Winner(Player* playersList, int PlayerCount, int PlayerTurn, int turnCount){
    //vérification si un joueur a gagné
    int ended = 1, x, y;
    for (int i = 0; i < 8; i++){
        x = playersList[PlayerTurn].piece[i].position.x;
        y = playersList[PlayerTurn].piece[i].position.y;
        if ((PlayerTurn == 0 && y != 9) || (PlayerTurn == 1 && y != 0) || (PlayerTurn == 2 && x != 0) || (PlayerTurn == 3 && x != 9)) ended = 0;
    }
    if (ended) return PlayerTurn+1; //retour de l'id du joueur +1 

    //joueur a perdus si un de ses pions est encore dans sa base après le 30e tour
    if (turnCount > 30){
        for (int i = 0; i < 8; i++){
            x = playersList[PlayerTurn].piece[i].position.x;
            y = playersList[PlayerTurn].piece[i].position.y;
            if ((PlayerTurn == 0 && y == 0) || (PlayerTurn == 1 && y == 9) || (PlayerTurn == 2 && x == 9) || (PlayerTurn == 3 && x == 0)) ended = 0;
        }
        if (ended == 0) return PlayerTurn + PlayerCount + 1; //retour de l'id du joueur +nombre de joueurs+1
    }

    //vérification égalité si un pion retourne sur la même case 3 fois
    for (int i = 0; i < PlayerCount; i++){
        for (int j = 0; j < 8; j++){
            for (int k = 0; k < 100; k++){
                if (playersList[i].piece[j].history[k] >= 3) return 0; //retour 0
            }
        }
    }
    return -1; //si il ne se passe rien, retourne -1
}

/*Fonction qui regarde si un joueur n'as aucun déplacement possible, pour sauter son tour*/
int cantDoSth(Player *playerList, int playerCount, int playerTurn){
    SDL_Point test[30];
    int lenTest;
    for (int i = 0; i < 8; i++){
        lenTest = 0;
        validMove(playerList[playerTurn].piece[i], playerList, playerCount, playerTurn, playerList[playerTurn].piece[i].position, test, &lenTest, 0);
        if (lenTest != 0) return 0;
    }
    return 1;
}