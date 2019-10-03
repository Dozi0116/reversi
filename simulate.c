#include <stdio.h>
#include "reversi.h"
#include "logic.h"

int main(void) {
    Game game;
    game_init(&game);
    int pos[2];
    
    // ゲームで双方のプレイヤーが使うロジックを突っ込む。
    void (*rogic[PLAYERS])(struct Game*, int []);
    rogic[BLACK] = player;
    rogic[WHITE] = bot_random;

    printf("%d\n", game.turn);

    while (TRUE) {
        show_board(&game);

        rogic[game.turn](&game, pos);

        printf("press -> (%d(%d), %c(%d))\n", pos[0], pos[0], pos[1] + 'A' - 1, pos[1]);
        put_stone(&game, pos, game.turn);

        if (next_turn(&game) == TRUE) {
            break;
        }
    }

    printf("game set!\n");
    show_board(&game);

    
    return 0;
}