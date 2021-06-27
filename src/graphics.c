#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <string.h>
#include "../headers/types.h"
#include "../headers/functions.h"

/*Fichier contenant les fonctions utiles à l'affichage à l'écran*/

//couleurs de la grille
SDL_Color blue = {50, 150, 200, 255};
SDL_Color white = {255, 255, 255, 255};
SDL_Color black = {0, 0, 0, 100};

char * rules = "But du jeu:\n\
  Le but du jeu est de faire traverser tous ses pions de l'autre côté du plateau.\n\n\
Départ:\n\
  Chaque joueur possède 8 pions:\n\
 \t- les carrés de déplacent horizontalement et verticalement\n\
 \t- les losanges se déplacent en diagonale\n\
 \t- les triangles se déplacent en diagonale vers l'avant et tout droit vers l'arrière\n\
 \t- les cercles se déplacent dans toutes les directions\n\n\
Déroulement d'une partie:\n\
   Les joueurs jouent à tour de rôle, deux pions ne peuvent pas occuper la même case.\n\
 Les pions peuvent se déplacer d'une case ou sauter au dessus d'un pion, sans le \"manger\"\n\
 Tous les pions restent donc jusqu'à la fin de la partie. Un pion ne peut pas finir son tour\n\
 sur une case en bordure mais peut transiter par une case en bordure si il la quitte dans le \n\
 même tour\n\n\
Fin du jeu:\n\
   Le jeu se termine lorsque:\n\
\t - l'un des joueurs a réussi à réunir tous ses pions dans la zone d'arrivée \n\
\t   (la bordure opposée sans les coins), il est ainsi déclaré vainqueur\n\
\t - l'un des joueurs n'as pas libéré sa zone de départ après le 30e tour,\n\
\t   il a alors perdus la partie\n\
\t - un pion retourne à une même position 3 fois, la partie est déclarée nulle";

/*Fonction utilisée pour afficher la grille (renderer, Height)*/
void grid(SDL_Renderer * renderer, int Height){
    int caseLen = Height/10; //longeur de la case
    for (int i = 0; i < 10; i++){ //simple boucle
        for (int j = 0; j < 10; j++){
            if ((i+j)%2 == 0) SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a); //alternance de couleur
            else SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, white.a);
            SDL_Rect rect = {caseLen*i, caseLen*j, caseLen, caseLen}; //calcul du rectangle à remplir
            SDL_RenderFillRect(renderer, &rect); //affichage de ce rectangle
        }
    }
}

/*Fonction d'affichage des pions sur la grille (renderer, height, playerCount, playerList, PlayerTurn)*/
void pawn(SDL_Renderer * renderer, int Height, int PlayerCount, Player * PlayersList, int playerTurn){
    for (int i = 0; i < PlayerCount; i++){ //boucle qui parcours tous les joueurs
        for (int j = 0; j < 8; j++){ //boucle qui parcours tous les pions des joueurs
            int x = PlayersList[i].piece[j].position.x; //coordonée
            int y = PlayersList[i].piece[j].position.y;
            int player = i;
            char c;
            if (j == 0 || j == 7) c = 'c';
            if (j == 1 || j == 6) c = 't';
            if (j == 2 || j == 5) c = 'l';
            if (j == 3 || j == 4) c = 'r';
            char link[50];
            sprintf(link, "res/Pawn/%c%d.png", c, player); //récupération de l'image de la forme suivant le type de forme et le joueur
            SDL_Surface * image = IMG_Load(link);
            SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
            SDL_Rect pos = {x*Height/10 + Height/80, y*Height/10 + Height/80, 3*Height/40, 3*Height/40}; //position du pion
            SDL_RenderCopy(renderer, texture, NULL, &pos); //ajout de l'image sur le renderer
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(image);
            if (i != playerTurn){ //si c'est pas au tour du joueur, un film est rajouté devant
                SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 150);
                SDL_Rect Selected = {x*Height/10, y*Height/10, Height/10, Height/10};
                SDL_RenderFillRect(renderer, &Selected);
            }
        }
    }
}

/*affichage des possibilitées de jeu par un jeton (renderer, array[point], len, height, pion)*/
void displayMoves(SDL_Renderer * renderer, SDL_Point * test, int * len, int Height, Forme pion){
    if (pion.id == -1) return;
    SDL_SetRenderDrawColor(renderer, white.r, white.g, white.b, 150);
    SDL_Rect Selected = {pion.position.x*Height/10, pion.position.y*Height/10, Height/10, Height/10};
    SDL_RenderFillRect(renderer, &Selected);

    SDL_SetRenderDrawColor(renderer, black.r, black.g, black.b, black.a);
    for (int i = 0; i < *len; i++){
        SDL_Rect pos = {test[i].x*Height/10 + 3*Height/80, test[i].y*Height/10 + 3*Height/80, Height/40, Height/40};
        SDL_RenderFillRect(renderer, &pos);
    }
}

/*Messagebox poour connaitre le nombre de joueurs (humain)*/
int playerCountMessage(){
    //affichage des règles
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Règles du jeu de traverse", rules, NULL);
    //data des boutons
    const SDL_MessageBoxButtonData buttons[] = {
        { 0, 3, "4" },
        { 0, 2, "3" },
        { 0, 1, "2" },
        { 0, 0, "1" }
    };
    //couleurs
    const SDL_MessageBoxColorScheme colorScheme = {
        {
            { 255,   0,   0 },
            {   0, 255,   0 },
            { 255, 255,   0 },
            {   0,   0, 255 },
            { 255,   0, 255 }
        }
    };
    //nom de la fenètre, ombre de boutons ...
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "Nombre de joueurs", /* .title */
        "Combien de personnes vont jouer au jeu ?", /* .message */
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid;
    SDL_ShowMessageBox(&messageboxdata, &buttonid); //affichage de la boite de dialogue
    if (buttonid == -1) buttonid = 0;
    return buttonid + 1; //renvois du résultat
}

/*Affichage d'une messagebox pour savoir le nombre d'ordinateur (voir ci dessus pour le détail)*/
int botCountMessage(int playerCount){
    const SDL_MessageBoxButtonData buttons[] = {
        { 0, 0, "0" },
        { 0, 1, "1" },
        { 0, 2, "2" },
        { 0, 3, "3" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        {
            { 255,   0,   0 },
            {   0, 255,   0 },
            { 255, 255,   0 },
            {   0,   0, 255 },
            { 255,   0, 255 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "Nombre de bots", /* .title */
        "Combien de d'ordinateurs vont jouer ?", /* .message */
        5-playerCount, /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid;
    SDL_ShowMessageBox(&messageboxdata, &buttonid);
    if (buttonid == -1) buttonid = 0;
    return buttonid;
}

/*Fonction pour demander au joueur si il souhaite recommencer*/
int retry(){
    const SDL_MessageBoxButtonData buttons[] = {
        { 0, 0, "Non" },
        { 0, 1, "Oui" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
        {
            { 255,   0,   0 },
            {   0, 255,   0 },
            { 255, 255,   0 },
            {   0,   0, 255 },
            { 255,   0, 255 }
        }
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "Recommencer", /* .title */
        "Voulez vous recommencer ?", /* .message */
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };
    int buttonid;
    SDL_ShowMessageBox(&messageboxdata, &buttonid);
    if (buttonid == -1) buttonid = 0;
    return buttonid;
}

/*Fonction pour afficher un texte avec une certaine transparence*/
void fadeText(char * text, SDL_Renderer * renderer, SDL_Point pos, int alpha, int size){
    SDL_Color couleur = {150, 150, 150}; //sélection par défaut de la couleur
    if (strlen(text) == 19){ //si on affiche le tour d'un joueur, on change la couleur
        switch (text[18] - '0'){
            case 1:
                couleur.r = 237;
                couleur.g = 28;
                couleur.b = 36;
                break;
            case 2:
                couleur.r = 200;
                couleur.g = 6;
                couleur.b = 249;
                break;
            case 3:
                couleur.r = 28;
                couleur.g = 237;
                couleur.b = 36;
                break;
            case 4:
                couleur.r = 237;
                couleur.g = 220;
                couleur.b = 28;
                break;
        }
    }
    //récupération de la police d'écriture
    TTF_Font* Sans = TTF_OpenFont("res/kelson.ttf", size);
    SDL_Rect Message_rect; //rectange d'affichage

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, couleur); //surface où se trouvera le texte

    Message_rect.x = pos.x - surfaceMessage->w/2; //calcul de la taille du rectangle suivant la taille du texte
    Message_rect.y = pos.y - surfaceMessage->h/2;
    
    Message_rect.w = surfaceMessage->w; 
    Message_rect.h = surfaceMessage->h;

    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //création de la texture du texte et ajout au renderer
    SDL_SetTextureBlendMode(Message, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(Message, alpha*255/100);
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    TTF_CloseFont(Sans); //libération de l'espace mémoire
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void createButton(char * text, SDL_Renderer * renderer, SDL_Point pos){
    SDL_Color couleur = {50, 50, 50};
    TTF_Font* Sans = TTF_OpenFont("res/kelson.ttf", 16);
    SDL_Rect Message_rect; //rectange d'affichage

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, couleur); //surface où se trouvera le texte

    Message_rect.x = pos.x - surfaceMessage->w/2; //calcul de la taille du rectangle suivant la taille du texte
    Message_rect.y = pos.y - surfaceMessage->h/2;
    
    Message_rect.w = surfaceMessage->w; 
    Message_rect.h = surfaceMessage->h;

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &Message_rect);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //création de la texture du texte et ajout au renderer
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    TTF_CloseFont(Sans); //libération de l'espace mémoire
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}