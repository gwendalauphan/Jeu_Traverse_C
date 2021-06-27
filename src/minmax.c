#include <stdio.h>
#include "../headers/types.h"
#include "../headers/functions.h"
#include <SDL2/SDL.h>

/*Fichier contenant les fonction nécéssaire au fonctionnement du bot avec l'algorithme minmax avec étalage alpha-beta*/

/*fonction prioncipale récursive de l'algo*/
int minimax(Player * playerList, int hasLost[4], int playerCount, int turnCount, int currentPlayer, int alpha, int beta, int depth, int iaTurn){
    int robot = Winner(playerList, playerCount, iaTurn, turnCount); //vérification si la partie n'est pas finie pour l'ia
    if (robot-1 == iaTurn) return 1000; //l'ia a gagnée, elle gagne plein de points
    if (robot-1 == iaTurn + playerCount) return -1000; //l'ia a perdue, elle perds plein de points
    if (robot == 0) return -100; //en cas d'égalité, elle perds des points pour ladécourager de chercher l'égalitée
    
    //depth correspond à la vision de l'ia, c'est là où on peut regler la difficultée (/!\ à la performance ~ 50^depth points à vérifier)
    if (depth == 2){ // en cas de fin, calcul du score suivant la position des pions 
        int score = 0, tmp;
        switch (iaTurn){
            case 0:
                for (int i = 0; i < 8; i ++){
                    tmp = playerList[iaTurn].piece[i].position.y;
                    score += tmp;
                    if (tmp == 0) score -= 10;
                }
                break;
            case 1:
                for (int i = 0; i < 8; i ++){
                    tmp = playerList[iaTurn].piece[i].position.y;
                    score += 9 - tmp;
                    if (tmp == 9) score -= 10;
                }
                break;
            case 2:
                for (int i = 0; i < 8; i ++){
                    tmp = playerList[iaTurn].piece[i].position.x;
                    score += 9 - tmp;
                    if (tmp == 9) score -= 10;
                }
                break;
            case 3:
                for (int i = 0; i < 8; i ++){
                    tmp = playerList[iaTurn].piece[i].position.x;
                    score += tmp;
                    if (tmp == 0) score -= 10;
                }
                break;
        }
        return score;
    }

    int bestScore = 100000;
    int len = 0, score;
    SDL_Point test[100], oldPos;
    int nextPlayer = currentPlayer;
    do{
        nextPlayer = (nextPlayer + 1)%playerCount;
        if (nextPlayer == 0) turnCount ++;
    } while (hasLost[nextPlayer]);


    if (currentPlayer == iaTurn) //cas où on maximise (pour min et max voire structure dans BestMove)
    {
        bestScore *= -1;
        for (int i = 0; i < 8; i++){
            oldPos = playerList[iaTurn].piece[i].position;
            validMove(playerList[iaTurn].piece[i], playerList, playerCount, currentPlayer, playerList[iaTurn].piece[i].position, test, &len, 0);
            
            for (int j = 0; j < len; j++){
                playerList[iaTurn].piece[i].position = test[j];
                score = minimax(playerList, hasLost, playerCount, turnCount, nextPlayer, alpha, beta, depth + 1, iaTurn);
                playerList[iaTurn].piece[i].position = oldPos;
                bestScore = (bestScore>score)?bestScore:score;
                if (bestScore >= beta) return bestScore;
                alpha = (alpha>bestScore)?alpha:bestScore;
            }
            len = 0;
        }
    }
    else{ //cas où on minimise
        for (int i = 0; i < 8; i++){
            oldPos = playerList[currentPlayer].piece[i].position;
            validMove(playerList[currentPlayer].piece[i], playerList, playerCount, currentPlayer, playerList[currentPlayer].piece[i].position, test, &len, 0);
            
            for (int j = 0; j < len; j++){
                playerList[currentPlayer].piece[i].position = test[j];
                score = minimax(playerList, hasLost, playerCount, turnCount, nextPlayer, alpha, beta, depth + 1, iaTurn);
                playerList[currentPlayer].piece[i].position = oldPos;
                bestScore = (bestScore<score)?bestScore:score;
                if (alpha >= bestScore) return bestScore;
                beta = (beta<bestScore)?beta:bestScore;
            }

            len = 0;
        }
    }
    return bestScore;
}

void BestMove(Player * playerList, int hasLost[4], int playerCount, int turnCount, int iaTurn){
    //initialisation variables
    int BestScore = -10000;
    int len = 0, score, bestPiece, stuck = 1, alpha = -100000, beta = +10000; //initialisation des variables et de alpha et de beta
    SDL_Point test[100], oldPos, BestPos;
    int nextPlayer = iaTurn;
    //prochain joueur à jouer
    do{
        nextPlayer = (nextPlayer + 1)%playerCount;
        if (nextPlayer == 0) turnCount ++;
    } while (hasLost[nextPlayer]);
    //parcourts des pions
    for (int i = 0; i < 8; i++){
        oldPos = playerList[iaTurn].piece[i].position; //mémorisation de l'ancienne posiition
        validMove(playerList[iaTurn].piece[i], playerList, playerCount, iaTurn, playerList[iaTurn].piece[i].position, test, &len, 0); //calcul des déplacements possibles
        
        if (len != 0) stuck = 0;

        for (int j = 0; j < len; j++){ //boucle dans chaque déplacement possible
            playerList[iaTurn].piece[i].position = test[j]; //changement de la position
            playerList[iaTurn].piece[bestPiece].history[10 * test[j].y + test[j].x] ++; //et de l'historique
            score = minimax(playerList, hasLost, playerCount, turnCount, nextPlayer, alpha, beta, 0, iaTurn); //envois dans la fonction récursive de l'algorithme
            playerList[iaTurn].piece[bestPiece].history[10 * test[j].y + test[j].x] --; //réstauration à l'état par défaut
            playerList[iaTurn].piece[i].position = oldPos;
            if (score > BestScore){ //si on a un meilleur score, on mémorise la position et le pion
                BestScore = score;
                BestPos = test[j];
                bestPiece = i;
            }
            alpha = (alpha > BestScore)?alpha:BestScore; //changement de la valeur de alpha
        }
        len = 0;
    }
    if ( stuck == 0){ //si le joueur peut faire quelque chose, on effectue le meilleur déplacement calculé au dessus
        playerList[iaTurn].piece[bestPiece].position = BestPos;
        playerList[iaTurn].piece[bestPiece].history[10 * BestPos.y + BestPos.x] ++;
    }
}