/*
ゲームの過程を見るときに使うファイル。
1手ごとにボードが表示されて、どこに打ったかが表示される。
rogicの部分をplayerに変更すれば対戦もできる。

コンパイル方法
> gcc simulate.c book_evaluation.c logic.c reversi.c utility.c -lm
*/

#include <stdio.h>
#include "reversi.h"
#include "logic.h"
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

int main(void) {
    Game game;
    game_init(&game);
    int pos[2];
    
    // ゲームで双方のプレイヤーが使うロジックを突っ込む。
    void (*rogic[PLAYERS])(struct Game*, int []);
    rogic[BLACK] = bot_alpha_beta;
    rogic[WHITE] = bot_softmax;

    srand(12346);

    struct timeval before, after;
    double elapse;
    long int sec, usec;

    while (TRUE) {
        // show_board(&game);

        // 時間計測
        gettimeofday(&before, NULL);
        rogic[game.turn](&game, pos);
        gettimeofday(&after, NULL);

        if (before.tv_usec > after.tv_usec) {
            // 繰り下がり
            sec = after.tv_sec - before.tv_sec - 1;
            usec = 1000000 + after.tv_usec - before.tv_usec;
        } else {
            // 何もなし
            sec = after.tv_sec - before.tv_sec;
            usec = after.tv_usec - before.tv_usec;
        }

        if (game.turn == WHITE) {
            // printf("%ld.%ld -> %ld.%ld\n", before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);
            printf("%ld.%ld\n", sec, usec);
        }
        // printf("press -> (%d(%d), %c(%d))\n", pos[0], pos[0], pos[1] + 'A' - 1, pos[1]);
        put_stone(&game, pos, game.turn);

        if (next_turn(&game) == TRUE) {
            break;
        }
    }

    printf("game set!\n");
    show_board(&game);

    
    return 0;
}