#include <stdio.h>
#include "reversi.h"
#include "logic.h"
#include <time.h>

int main(void) {
    
    // ゲームで双方のプレイヤーが使うロジックを突っ込む。
    void (*rogic[PLAYERS])(struct Game*, int []);
    rogic[BLACK] = bot_softmax;
    rogic[WHITE] = bot_random;

    srand((unsigned int)time(NULL));
    // srand(0);

    int i, j, k, stone, win = 0, lose = 0;
    for (i = 0;i < 100;i++){
        Game game;
        game_init(&game);
        int pos[2];

        while (TRUE) {
            rogic[game.turn](&game, pos);

            put_stone(&game, pos, game.turn);

            if (next_turn(&game) == TRUE) {
                break;
            }
        }
        printf("game %d\n", i+1);

        stone = 0;
        for (j = 1;j <= BOARD_SIZE;j++) {
            for (k = 1;k <= BOARD_SIZE;k++) {
                if (game.board[j][k] == BLACK) stone++;
                else if (game.board[j][k] == WHITE) stone--;
            }
        }

        if (stone > 0) {
            win++;
            printf("win\n");
        } else if (stone < 0) {
            lose++;
            printf("lose\n");
        }
    }

    printf("win -> %d, lose -> %d\n", win, lose);
    
    return 0;
}