
void reset(int, Player *);
SDL_Point applyRotation(float, SDL_Point);
int isFree(Player *, int, SDL_Point);
void validMove(Forme, Player *, int, int, SDL_Point, SDL_Point arr[30], int *, int);
Forme *pawnAt(Player*, int, int, Forme);
int Winner(Player *, int, int, int);
int cantDoSth(Player *, int, int);
char *substr(char *, int, int);