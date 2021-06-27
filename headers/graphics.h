
int playerCountMessage();
int botCountMessage(int);
void grid(SDL_Renderer *, int);
void pawn(SDL_Renderer *, int, int, Player *, int);
void displayMoves(SDL_Renderer *, SDL_Point* , int *, int, Forme);
void fadeText(char *, SDL_Renderer *, SDL_Point, int, int);
int retry();
int createButton(char *, SDL_Renderer *, SDL_Point);