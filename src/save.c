#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "../headers/types.h"
#include "../headers/functions.h"

/*Fichier contenant les fonctions pour sauvegarder / charger uine partie*/

void Save(Player * playerList, int playerCount, int playerTurn, int hasLost[4], int turnCount){
    char command[120];

    time_t currentTime;
    struct tm *timeComp;
    
    time(&currentTime);
    timeComp = localtime(&currentTime);

    system("cd saves & md tmp & cd ../"); //création du dossier de sauvegarde

    FILE* file = fopen("saves/tmp/main", "w");
    fprintf(file, "%d\n%d\n%d\n%d %d %d %d", playerCount, playerTurn, turnCount, hasLost[0], hasLost[1], hasLost[2], hasLost[3]);
    fclose(file);

    for (int i = 0; i < playerCount; i ++){
        char path[50];
        sprintf(path, "saves/tmp/player%d", i);
        FILE* file = fopen(path, "w");
        for (int j = 0; j < 8; j++){
            char history[100] = "0";
            for (int k = 0; k < 100; k++){
                history[k] = playerList[i].piece[j].history[k] + 48;
            }
            fprintf(file, "%d %d\n%s\n", playerList[i].piece[j].position.x, playerList[i].piece[j].position.y, history);
        }
        fclose(file);
    }
    sprintf(command, "powershell Compress-Archive saves/tmp saves/date_%d-%02d-%02d_%02d_%02d_%02d_nbJoueurs_%d.zip\n", timeComp->tm_year + 1900,
                                                   timeComp->tm_mon + 1,
                                                    timeComp->tm_mday,
                                                    timeComp->tm_hour,
                                                    timeComp->tm_min,
                                                    timeComp->tm_sec,
                                                    playerCount);
    system(command); //création du zip
    system("cd saves & rmdir tmp /s /q & cd ../");
}

void Load(char * path, Player * playerList, int *playerCount, int *playerTurn, int hasLost[4], int *turnCount){
    //récupérer et afficher les différentes sauvegardes
    char command[120];
    sprintf(command, "powershell Expand-Archive saves/%s saves", path);
    system(command); //décompression du zip

    FILE* file = fopen("saves/tmp/main", "r");
    fscanf(file, "%d\n%d\n%d\n%d %d %d %d", playerCount, playerTurn, turnCount, &hasLost[0], &hasLost[1], &hasLost[2], &hasLost[3]);
    fclose(file);
    reset(*playerCount, playerList);
    for (int i = 0; i < *playerCount; i ++){
        char path[50];
        sprintf(path, "saves/tmp/player%d", i);
        FILE* file = fopen(path, "r");
        for (int j = 0; j < 8; j++){
            char history[300];
            fscanf(file, "%d %d\n%s\n", &playerList[i].piece[j].position.x, &playerList[i].piece[j].position.y, history);
            for (int k = 0; k < 100; k++){
                playerList[i].piece[j].history[k] = history[k] - 48;
            }
        }
        fclose(file);
    }
    system("cd saves & rmdir tmp /s /q & cd ../"); //suppression du dossier tmp
}