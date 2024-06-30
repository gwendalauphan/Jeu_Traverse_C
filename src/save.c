#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <time.h>
#include "../headers/types.h"
#include "../headers/functions.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>  // For mkdir on UNIX
#endif
/*Fichier contenant les fonctions pour sauvegarder / charger uine partie*/

void Save(Player * playerList, int playerCount, int playerTurn, int hasLost[4], int turnCount) {
    char command[256];
    char path[50];
    
    char cwd[1024];  // Buffer pour le chemin actuel

    // Obtenir le répertoire de travail actuel
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return;  // Gérer l'erreur si nécessaire
    }

    time_t currentTime;
    struct tm *timeComp;

    time(&currentTime);
    timeComp = localtime(&currentTime);

    // Cross-platform directory creation and cleanup
    #ifdef _WIN32
    system("md saves\\tmp");
    #else
    mkdir("saves/tmp", 0755); // Use mkdir directly from sys/stat.h on UNIX
    #endif

    FILE* file = fopen("saves/tmp/main", "w");
    if (file == NULL) {
        perror("Failed to open file for writing");
        return;
    }
    fprintf(file, "%d\n%d\n%d\n%d %d %d %d", playerCount, playerTurn, turnCount, hasLost[0], hasLost[1], hasLost[2], hasLost[3]);
    fclose(file);

    for (int i = 0; i < playerCount; i++) {
        sprintf(path, "saves/tmp/player%d", i);
        FILE* file = fopen(path, "w");
        if (file == NULL) {
            perror("Failed to open player file for writing");
            continue;
        }
        for (int j = 0; j < 8; j++) {
            char history[100];
            for (int k = 0; k < 100; k++) {
                history[k] = playerList[i].piece[j].history[k] + 48;
            }
            fprintf(file, "%d %d\n%s\n", playerList[i].piece[j].position.x, playerList[i].piece[j].position.y, history);
        }
        fclose(file);
    }

    #ifdef _WIN32
    // Windows command to create a ZIP archive using PowerShell
    sprintf(command, "powershell -command \"Compress-Archive -Path './tmp' -DestinationPath 'date_%d-%02d-%02d_%02d_%02d_%02d_nbJoueurs_%d.zip' -Force\"",
            timeComp->tm_year + 1900, timeComp->tm_mon + 1, timeComp->tm_mday, timeComp->tm_hour, timeComp->tm_min, timeComp->tm_sec, playerCount);
    system(command);

    // Windows command to remove the temporary directory
    sprintf(command, "rmdir /s /q saves\\tmp");
    system(command);
    #else
    // Unix command to create a ZIP archive
    sprintf(command, "cd saves && zip -r date_%d-%02d-%02d_%02d_%02d_%02d_nbJoueurs_%d.zip ./tmp",
            timeComp->tm_year + 1900, timeComp->tm_mon + 1, timeComp->tm_mday, timeComp->tm_hour, timeComp->tm_min, timeComp->tm_sec, playerCount);
    system(command);

    // Unix command to remove the temporary directory
    system("rm -rf saves/tmp");
    #endif
}

void Load(char *path, Player *playerList, int *playerCount, int *playerTurn, int hasLost[4], int *turnCount) {
    char command[120];
    #ifdef _WIN32
    sprintf(command, "powershell Expand-Archive saves/%s saves", path);
    #else
    sprintf(command, "unzip -o saves/%s -d saves", path); // Option -o pour écraser les fichiers existants
    #endif
    system(command); // Décompression du zip

    FILE* file = fopen("saves/tmp/main", "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    fscanf(file, "%d\n%d\n%d\n%d %d %d %d", playerCount, playerTurn, turnCount, &hasLost[0], &hasLost[1], &hasLost[2], &hasLost[3]);
    fclose(file);
    //print the values
    
    reset(*playerCount, playerList);
    for (int i = 0; i < *playerCount; i++) {
        char path[50];
        sprintf(path, "saves/tmp/player%d", i);
        FILE* file = fopen(path, "r");
        if (!file) {
            perror("Failed to open player file");
            continue;
        }
        for (int j = 0; j < 8; j++) {
            char history[300];
            fscanf(file, "%d %d\n%s\n", &playerList[i].piece[j].position.x, &playerList[i].piece[j].position.y, history);
            for (int k = 0; k < 100; k++) {
                playerList[i].piece[j].history[k] = history[k] - 48;
            }
        }
        fclose(file);
    }

    #ifdef _WIN32
    system("cd saves & rmdir tmp /s /q & cd ../");
    #else
    system("rm -rf saves/tmp"); // Suppression du dossier tmp sous Linux
    #endif
    printf("Load successful\n");
}