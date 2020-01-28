#include <stdio.h>
#include "reversi.h"
#include "logic.h"
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("usage: ./soft_test <seed>\n");
        printf("seed -> random seed\n");

        exit(1);
    }

    srand(atoi(argv[1]));
    // ゲームで双方のプレイヤーが使うロジックを突っ込む。
    void (*rogic[PLAYERS])(struct Game*, int []);
    int color;

    Game game;

    const int match_max = 500;

    int total_win = 0, total_lose = 0;
    int i, j, k, stone, win = 0, lose = 0, result = 0;

    int pos[2];
    int wins[2];
    int order;

    rogic[BLACK] = bot_softmax;
    rogic[WHITE] = bot_softmax;

    for (order = 1;order <= 2;order++) {
        win = 0;
        if (order == 1) color = BLACK;
        else color = WHITE;

        for (match = 0;match < match_max;match++) {
            game_init(&game);

            fprintf(stderr, "game %d\n", match+1);

            while (TRUE) {
                if (
                    order == 1 && game.turn == BLACK ||
                    order == 2 && game.turn == WHITE
                ) {
                    softmax_T = 61;
                } else {
                    softmax_T = 1;
                }
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
