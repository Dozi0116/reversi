#include <stdio.h>
#include "reversi.h"
#include "logic.h"
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    
    // ゲームで双方のプレイヤーが使うロジックを突っ込む。
    void (*rogic[PLAYERS])(struct Game*, int []);
    rogic[BLACK] = bot_softmax;
    rogic[WHITE] = bot_random;

    srand(12346);

    const int match = 4;
    int T;

    printf("%d, %d, %d, %d, %d\n", rand(), rand(), rand(), rand(), rand());

    for (T = 1;T <= 3;T++){
        softmax_T = T;
        printf("T = %lf\n", softmax_T);
        int pos, total_win = 0, total_lose = 0;
        int color;
        for (pos = 0;pos <= 1;pos++){
            if (pos == 0) {
                rogic[BLACK] = bot_softmax;
                rogic[WHITE] = bot_alpha_beta;
                color = BLACK;
                printf("softmax 1st\n");
            } else {
                rogic[BLACK] = bot_alpha_beta;
                rogic[WHITE] = bot_softmax;
                color = WHITE;
                printf("softmax 2nd\n");
            }
            int i, j, k, stone, win = 0, lose = 0, result = 0;

            for (i = 0;i < match / 2;i++){
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
                // printf("game %d\n", i+1);

                result = count_board(game.board, color);
                show_board(&game);
                if (result == 1) win++;
                else if (result == -1) lose++;
            }

            printf("\twin -> %d, lose -> %d, draw -> %d\n", win, lose, match / 2 - win - lose);
            total_win += win;
            total_lose += lose;
        }

        printf("total win -> %d, lose -> %d\n", total_win, total_lose);
    }
        
    return 0;
}