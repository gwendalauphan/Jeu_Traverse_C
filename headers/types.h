
#include <SDL2/SDL.h>

//structure pour les pions des joueurs
typedef struct {
    int nb_deplacement; //nombre de moves possibles
    SDL_Point deplacement[8]; //liste des moves possibles
    SDL_Point position; //position actuelle du pion sur le jeu
    int id;             //type de forme (0: carre, 1: triangle, 2:losange, 3:cercle)
    int history[100];
} Forme;

//structure pour les joueurs
typedef struct {
    float rotation; //rotation pour savoir dans quelle direction vont les pions: (cos(a)x - sin(a)y; sin(a)x + cos(a)y)
    Forme piece[8]; //triangle*2 carre*2 losange*2 cercle*2
} Player;
