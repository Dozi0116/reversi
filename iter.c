#include <stdio.h>
#include "reversi.h"
#include "logic.h"
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("usage: ./test <T> <seed>\n");
        printf("T -> 1-100\n");
        printf("seed -> random seed\n");

        exit(1);
    }

    srand(atoi(argv[2]));
    softmax_T = atoi(argv[1]);
    // ゲームで双方のプレイヤーが使うロジックを突っ込む。
    void (*rogic[PLAYERS])(struct Game*, int []);
    int color;

    Game game;

    const int match_max = 100;

    int total_win = 0, total_lose = 0;
    int i, j, k, stone, win = 0, lose = 0, result = 0;

    int pos[2];
    int wins[2];
    int order;

    for (order = 1;order <= 2;order++) {
        if (order == 1) {
            // softmax first
            rogic[BLACK] = bot_softmax;
            rogic[WHITE] = bot_alpha_beta;
            color = BLACK;
        } else if (order == 2) {
            // softamx second
            rogic[BLACK] = bot_alpha_beta;
            rogic[WHITE] = bot_softmax;
            color = WHITE;
        }

        win = 0;
        lose = 0;

        for (match = 0;match < match_max;match++) {
            game_init(&game);

            fprintf(stderr, "game %d\n", match+1);

            while (TRUE) {
                rogic[game.turn](&game, pos);

                put_stone(&game, pos, game.turn);

                if (next_turn(&game) == TRUE) {
                    break;
                }
            }

            result = count_board(game.board, color);
            // show_board(&game);
            if (result == 1) win++;
            else if (result == -1) lose++;
        }

        fprintf(stderr, "\twin -> %d, lose -> %d, draw -> %d\n", win, lose, match_max - win - lose);
        wins[order-1] = win;
    }

    printf("%d, %d, %d, %d\n", (int)softmax_T, wins[0], wins[1], wins[0] + wins[1]);
        
    return 0;
}
