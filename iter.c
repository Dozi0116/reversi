#include <stdio.h>
#include "reversi.h"
#include "logic.h"
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    if (argc != 4) {
        printf("usage: ./test <order> <T> <seed>\n");
        printf("order -> 1: first, 2: second\n");
        printf("T -> 1-100\n");
        printf("seed -> random seed\n");

        exit(1);
    }

    srand(atoi(argv[3]));
    softmax_T = atoi(argv[2]);
    // ゲームで双方のプレイヤーが使うロジックを突っ込む。
    void (*rogic[PLAYERS])(struct Game*, int []);
    const int order = atoi(argv[1]);
    int color;
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
    } else {
        printf("order error.\n");
    }

    Game game;


    const int match_max = 500;

    int total_win = 0, total_lose = 0;
    int i, j, k, stone, win = 0, lose = 0, result = 0;

    int pos[2];

    for (match = 0;match < match_max;match++){
        game_init(&game);

        // printf("game %d\n", i+1);

        while (TRUE) {
            if (match == 70 && game.stone_num == 44) {
                printf("hoge\n");
                show_board(&game);
            }
            // printf("\tturn %d\n", game.stone_num-3);
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

    printf("\twin -> %d, lose -> %d, draw -> %d\n", win, lose, match_max - win - lose);
        
    return 0;
}
