#include <stdlib.h>
#include <stdio.h>
#include "reversi.h"

const int FLAG[DIRECTION_SIZE] = {
         UP_LEFT,   UP_CENTER,   UP_RIGHT,
        MID_LEFT,               MID_RIGHT,
       DOWN_LEFT, DOWN_CENTER, DOWN_RIGHT
};

const int DIRECTION[DIRECTION_SIZE][2] = {
       {-1, -1}, {-1,  0}, {-1,  1},
       { 0, -1},           { 0,  1},
       { 1, -1}, { 1,  0}, { 1,  1}
};

void game_init(Game *game) {
    int y, x;
    // printf("Reversi Game\n"); // このprintf文がないと動かない
    for (y = 0; y < BOARD_SIZE+2;y++){
        for (x = 0;x < BOARD_SIZE+2;x++){
            game -> board[y][x] = EMPTY;
            game -> reverse[y][x] = 0;
        }
    }

    game -> turn = BLACK;
    game -> stone_num = 0;

    game -> score = (INT_LIST **)malloc(sizeof(INT_LIST *) * (BOARD_SIZE * BOARD_SIZE));
    int i;
    for (i = 0; i < (BOARD_SIZE * BOARD_SIZE);i++){
        game -> score[i] = new_int_list(1);
    }
}

void make_putlist(struct Game *game,
                  int player) {
    /*
    playerとboardからひっくり返せる場所を見つけて返す。
    */

   int y, x;
   int dy, dx;
   int i;

    for (y = 1;y <= BOARD_SIZE;y++) {
        for (x = 1;x <= BOARD_SIZE;x++) {
            game -> reverse[y][x] = 0; // 初期化
            if (game -> board[y][x] != EMPTY) continue;
            for (i = 0;i < 8;i++) {
                dy = y + DIRECTION[i][0];
                dx = x + DIRECTION[i][1];
                if (game -> board[dy][dx] == -player) {
                    do {
                        dy += DIRECTION[i][0];
                        dx += DIRECTION[i][0];
                    } while (game -> board[dy][dx] == -player);
                    
                    if (game -> board[dy][dx] == player) {
                        game -> reverse[y][x] |= FLAG[i];
                    }
                }
            }
        }
    }
}


void put_stone(struct Game *game,
               int pos[],
               int player) {
    /*
    boardのposにplayerの石を置く。
    */

   int y = pos[0], x = pos[1];
   int dy, dx;
   int i;
   int score_index = 0;

   game -> board[y][x] = player;
   game -> score[game -> stone_num][score_index].data = pos2i(y, x);
   score_index++;

   for (i = 0; i < DIRECTION_SIZE;i++) {
       if ((game -> reverse[y][x] & FLAG[i]) != 0) {
           // その方向に返すことができる
            dy = y + DIRECTION[i][0];
            dx = x + DIRECTION[i][1];
            do {
                game -> board[dy][dx] = player;
                int_list_append(game -> score[game -> stone_num], pos2i(y, x));
                score_index++;
                dy += DIRECTION[i][0];
                dx += DIRECTION[i][0];
            } while (game -> board[dy][dx] == -player);           
       }
   }

    int_list_append(game -> score[game -> stone_num], -1);
    (game -> stone_num)++;
}