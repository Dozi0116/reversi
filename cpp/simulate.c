#include <stdio.h>
#include "reversi.h"

int main(void) {
    Game game;
    Game *game_ptr = &game;
    game_init(game_ptr);
    printf("game -> turn : %d\n", game_ptr -> turn);
    return 0;
}