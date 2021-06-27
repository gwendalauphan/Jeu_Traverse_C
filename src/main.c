#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <string.h>
#include "../headers/types.h"
#include "../headers/functions.h"
#include "../headers/graphics.h"
#include "../headers/minimax.h"
#include "../headers/save.h"
#define FPS 150

/*Fichier avec la fonction principale du programme*/

//fonction principale
int main(int argc, char *argv[]){
    start:
    ;
    Player PlayersList[4]; //liste contenant les données des joueurs
    int PlayerCount = playerCountMessage(); //récupération du nombre de joueurs
    int BotCount = botCountMessage(PlayerCount); //récupération du nombre de bots
    if (PlayerCount == 1 && BotCount == 0) BotCount = 1; //on oblige a avoir au moins un joueur contre un bot
    int hasLost[4] = {0,0,0,0}; //initialisation de la liste des états des joueurs (0 joueur, -1 bot, 1 mort)
    for (int i = PlayerCount; i< PlayerCount+BotCount; i ++){
        hasLost[i] = -1; //pour chaque bot, on l'joute dans la liste
    }
    PlayerCount += BotCount; //incrémentation du nombre de bot au total de joueurs 
    reset(PlayerCount, PlayersList); //initialisation des joueurs
    
    //initialisation de la fenetre et de la police
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    int Height = 800; //hauteur de la fenètre, peut être modifié pour un écran plus petit
    SDL_Point center = {Height/2, Height/2}; //centre de l'écran
    
    //position du label affichant le FPS
    SDL_Point FPS_label_Point = {19*Height/20, Height/40}, turnCountPoint = {9*Height/8, Height/4}, playerTurnPoint = {9*Height/8, Height/2};

    //création de la fenêtre de jeu principale
    SDL_Window *window = SDL_CreateWindow("Jeu de la Traversée", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Height*1.25, Height, SDL_WINDOW_SHOWN);
    //création du renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int continuer = 1, beginRender = 0, lastRender = 0, dtime = 0; //variable de la bnoucle principale, variable pour garder un fps constant
    SDL_Event event; //event SDL (clique souris)
    char textToDisplay[40] = "Bienvenue"; //variable contenant le texte a afficher
    float alphaValue = 100.0; //valeur alpha (transparence) du texte à afficher
    char fps_label[30] = "fps:", playerTurnLabel[30] = "Au tour du joueur 1", turnCountLabel[10] = "Tour 1"; //variable pour l'affichage des données

    time_t start = time(NULL), now;
    int compteur = 0, len, canclick = 1, playerTurn = 0, turnCount = 1;
    int result = -1; //variable de vérification d'un gagnant eventuel (-1: personne, n>0: nom gagnant, 0: égalité, -2 affichage de fin, n>playerCount: n-playerCount a perdus)
    Forme NotPoint = {.id = -1}; //pointeur vers un "non" point
    Forme * toDisplayMoves = &NotPoint, *tmp; //pion a afficher, variable de process
    SDL_Point mousePt, test[30]; //point là où le joueur à cliqué

    SDL_Point loadPoint = {23*Height/20, 19*Height/20}, savePoint = {23*Height/20, 17*Height/20};
    char loadPath[50] = "0";

    //boucle principale
    while (continuer){

        //tour d'un bot si l'id est -1
        if (hasLost[playerTurn] == -1){
            BestMove(PlayersList, hasLost, PlayerCount, turnCount, playerTurn); //fonction qui joue le meilleur coup
            result = Winner(PlayersList, PlayerCount, playerTurn, turnCount); //vérification de l'état du bot
            //au tour de la prochaine personne qui n'as pas perdus
            do {
                playerTurn = (playerTurn + 1)%PlayerCount; //sélection de l'id suivant
                if (playerTurn == 0) { //si on reviens au premier, c'est qu'on a complété un tour
                    turnCount ++;
                    sprintf(turnCountLabel, "Tour %d", turnCount);
                }
            } while (hasLost[playerTurn] == 1 || cantDoSth(PlayersList, PlayerCount, playerTurn) == 1); //le prochain joueur ne doit pas avoir perdus et doit pouvoir faire un mouvement
            sprintf(playerTurnLabel, "Au tour du joueur %d", playerTurn+1);
        }

        //si la partie est finie et que le joueur veut recommencer, on relance l'application
        if (result == -2) {
            if (retry()){
                TTF_Quit(); //destruction du programme
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                goto start;
            }
            else{
                continuer = 0;
            }
        }
        
        //récupération des évènements
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) continuer = 0; //si il faut quitter, on casse la boucle
        if (result == -1) switch(event.type) { //si la partie n'est pas finie 
            case SDL_MOUSEBUTTONDOWN:
                canclick = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                if (canclick == 1) {//condition pour éviter que la fonction soit appelée plusieurs fois en un click
                    mousePt.x = 10*event.button.x/Height; //récupération de l'emplacement de la souris
                    mousePt.y = 10*event.button.y/Height;
                    tmp = pawnAt(&PlayersList[playerTurn], mousePt.x, mousePt.y, NotPoint); //conversion en point
                    if (toDisplayMoves->position.x == mousePt.x && toDisplayMoves->position.y == mousePt.y && toDisplayMoves->id != -1){
                        toDisplayMoves = &NotPoint; //déselection d'un point sélectionné
                        len = 0;
                    }
                    else if (tmp->id != -1) {
                        toDisplayMoves = tmp;
                        len = 0;
                        validMove(*toDisplayMoves, PlayersList, PlayerCount, playerTurn, toDisplayMoves->position, test, &len, 0); //recherche des moves possibles
                    }
                    else{
                        for (int i = 0; i < len; i++){ //vérificaton si le joueur a cliqué sur un des coups possibles du pion sélectionné
                            if (test[i].x == mousePt.x && test[i].y == mousePt.y){ //le joueur a cliqué sur un coup possible
                                toDisplayMoves->position = mousePt; //ajustement de la possition du pion sélectionné
                                toDisplayMoves->history[10*mousePt.x + mousePt.y] ++; //incrémentation du nombre de fois qu'il est tombé sur cette case
                                toDisplayMoves = &NotPoint; //réinitialisation du pion à afficher
                                result = Winner(PlayersList, PlayerCount, playerTurn, turnCount); //recherche d'une éventuelle vitoire /défaite
                                len = 0; //réinitialisation de la liste des moves possibles
                                //au tour de la prochaine personne qui n'as pas perdus (voir au dessus)
                                do {
                                    playerTurn = (playerTurn + 1)%PlayerCount;
                                    if (playerTurn == 0) {
                                        turnCount ++;
                                        sprintf(turnCountLabel, "Tour %d", turnCount);
                                    }
                                } while (hasLost[playerTurn] == 1 || cantDoSth(PlayersList, PlayerCount, playerTurn) == 1);
                                sprintf(playerTurnLabel, "Au tour du joueur %d", playerTurn+1);
                            }
                        }
                    }
                    
                    //bouton de load
                    if (event.button.x > Height && event.button.x < 11*Height/10 && event.button.y > 9*Height/10 && event.button.y < Height){
                        SDL_MinimizeWindow(window);
                        goto loadWindow;
                        returnLoad:
                        ;
                        if (loadPath[0] != '0'){
                            Load(loadPath, PlayersList, &PlayerCount, &playerTurn, hasLost, &turnCount);
                        }
                        SDL_RestoreWindow(window);
                    }
                    //bouton de save
                    if (event.button.x > Height && event.button.x < 11*Height/10 && event.button.y > 8*Height/10 && event.button.y < 9*Height/10){
                        Save(PlayersList, PlayerCount, playerTurn, hasLost, turnCount);
                    }
                }
                canclick = 0;
                break;
        }
        else if (result != -2) { //si jamais un des joueur atteint une fin
            if (result == 0) strcpy(textToDisplay,"Il y a egalite"); //si il y a égalité, affichage du message
            else if (result <= PlayerCount) sprintf(textToDisplay, "Le joueur %d a gagne", result); //si le jouueur à gagné, affichage du message
            else {
                sprintf(textToDisplay, "Le joueur %d a perdus", result - PlayerCount);
                hasLost[result - PlayerCount - 1] = 1; //si un joueur a perdus, son état est changé dans la liste
            }
            int sum = 0; //somme pour voir combien de personnes restent en jeu
            for (int i = 0; i < PlayerCount; i++) sum += (hasLost[i] == 0 || hasLost[i] == -1);
            if (sum == 1){ //si il n'en reste qu'une, elle a gagnée
                for (int i = 0; i<PlayerCount; i++){
                    if (hasLost[i] != 1) result = i+1;
                }
            }

            if (result == 0 || (result > 0 && result <= PlayerCount)){ //Le jeu est finis
                result = -2;
            }
            else {
                result = -1; //la partie continue
            }
            alphaValue = 100; //modification de la valeur de l'alpha d'affichage
        }

        /*actualisation de la fenêtre*/
        grid(renderer, Height); //affichage de la grille
        pawn(renderer, Height, PlayerCount, PlayersList, playerTurn); //afficahe des pions

        if (toDisplayMoves->id != -1) displayMoves(renderer, test, &len, Height, *toDisplayMoves); //affichage du pion sélectionné
        
        if (alphaValue > 0){ //affichage d'un éventuel texte avec fondus
            fadeText(textToDisplay, renderer, center, alphaValue, 70);
            if (result != -2) alphaValue -= 0.4;
            else alphaValue -= 0.1;
        }
        
        //affichage des informations
        fadeText(fps_label, renderer, FPS_label_Point, 75, 15);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect rect = {Height, 0, Height/4, Height};
        SDL_RenderFillRect(renderer, &rect);

        fadeText(turnCountLabel, renderer, turnCountPoint, 100, 15);
        fadeText(playerTurnLabel, renderer, playerTurnPoint, 100, 15);

        createButton("Save", renderer, loadPoint);
        createButton("Load", renderer, savePoint);

        SDL_RenderPresent(renderer);


        //regulation de la vitesse de rafraichissement pour ne pas surcharger le processeur
        beginRender = SDL_GetTicks();
        dtime = beginRender - lastRender;
    
        if(dtime < 0) dtime = 0;
        if(dtime < 1000 / FPS)
        SDL_Delay( (1000 / FPS) - dtime);
    
        lastRender = SDL_GetTicks();

        //calcul du nombre d'images calculées lors de la dernière seconde (FPS: Frame Per Second)
        now = time(NULL);
        compteur ++;
        if (now - start == 1){
            sprintf(fps_label, "Fps: %d", compteur);
            start = now;
            compteur = 0;
        }      

    }

    goto Skip;
    loadWindow:
    ;

    //création d'une liste des sauvegardes dans le dossier saves
    DIR * rep = opendir ("./saves");
    int n = 0, ind = 0;
    char listPath[100][100];
  
    struct dirent * ent;

    while ((ent = readdir (rep)) != NULL){
        if (ent->d_name[0] != '.'){
            n ++;
            strcpy(listPath[ind++], ent->d_name);
        }
    }


    closedir (rep);
    int done = 1, Canclick = 0;
    SDL_Window *SubWindow = SDL_CreateWindow("Partie à charger", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              400, 500, SDL_WINDOW_SHOWN);
    //création du renderer
    SDL_Renderer * loadRenderer = SDL_CreateRenderer(SubWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(loadRenderer, SDL_BLENDMODE_BLEND);
    SDL_RaiseWindow(SubWindow);
    SDL_Point Subcenter = {200, 100}, pointInput = {160, 450}, pointEntry = {315, 450}, pointValider = {315, 475};
    SDL_Event eventInput;
    char input[10];

    SDL_StartTextInput();
    while (done){
        SDL_PollEvent(&eventInput);
        if (eventInput.type == SDL_WINDOWEVENT && eventInput.window.event == SDL_WINDOWEVENT_CLOSE) done = 0;
        else if (eventInput.type == SDL_TEXTINPUT || eventInput.type == SDL_KEYUP){
            if (eventInput.type == SDL_KEYUP && eventInput.key.keysym.sym == SDLK_BACKSPACE && strlen(input) > 0){
                
                if (strlen(input) == 1) input[0] = '\0';
                else input[strlen(input)-1] = '\0';
            }
            else if (eventInput.type == SDL_TEXTINPUT){
                sprintf(input, "%s%s", input, eventInput.text.text);
            }
        }
        if (eventInput.type == SDL_MOUSEBUTTONDOWN) Canclick = 1;
        else if (eventInput.type == SDL_MOUSEBUTTONUP){
            if (Canclick == 1) {
                int x = eventInput.button.x;
                int y = eventInput.button.y;
                if (x > pointValider.x -40 && x < pointValider.x +40 && y > pointValider.y -20 && y < pointValider.y +20){
                    if (strcmp(input, "") && atoi(input)-1 < n){
                        Load(listPath[atoi(input)-1], PlayersList, &PlayerCount, &playerTurn, hasLost, &turnCount);
                        //actualisation affichage
                        sprintf(turnCountLabel, "Tour %d", turnCount);
                        sprintf(playerTurnLabel, "Au tour du joueur %d", playerTurn+1);
                        done = 0;
                    }
                }
            }
            Canclick = 0;
        }
               

        SDL_SetRenderDrawColor(loadRenderer, 50, 50, 50, 255);
        SDL_RenderFillRect(loadRenderer, NULL);

        char text[500];
        sprintf(text, "Liste des parties sauvegardes:");
        fadeText(text, loadRenderer, Subcenter, 100, 20);
        for (int i = 0; i < n; i++){
            sprintf(text, "%d->%s", i+1, listPath[i]);
            SDL_Point point = {Subcenter.x, Subcenter.y + 25 * (i+1)};
            fadeText(text, loadRenderer, point, 100, 18);
        }

        fadeText("Entrer un numero de sauvegarde :", loadRenderer, pointInput, 100, 18);
        char tmp[10];
        sprintf(tmp, " %s", input);
        fadeText(tmp, loadRenderer, pointEntry, 100, 18);
        createButton("Valider", loadRenderer, pointValider);
        SDL_RenderPresent(loadRenderer);
        SDL_Delay(100);
    }
    SDL_StopTextInput();
    SDL_DestroyRenderer(loadRenderer);
    SDL_DestroyWindow(SubWindow);

    goto returnLoad;
    Skip:
    ;
    //sortie de la fenètre
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}