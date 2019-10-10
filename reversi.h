#ifndef REVERSI_H
#define REVERSI_H

#include "const.h"
#include "list.h"
#include <stdlib.h>

#define board_up_left(y, x) board[y-1][x-1]
#define board_up_center(y, x) board[y-1][x]
#define board_up_right(y, x) board[y-1][x+1]
#define board_mid_left(y, x) board[y][x-1]
#define board_mid_center(y, x) board[y][x]
#define board_mid_right(y, x) board[y][x+1]
#define board_down_left(y, x) board[y+1][x-1]
#define board_down_center(y, x) board[y+1][x]
#define board_down_right(y, x) board[y+1][x+1]

#define pos2i(y, x) y*8+x
#define i2row(i) i/(BOARD_SIZE+2)
#define i2col(i) i%(BOARD_SIZE+2)

typedef struct Game{
    char board[BOARD_SIZE+2][BOARD_SIZE+2];
    unsigned char reverse[BOARD_SIZE+2][BOARD_SIZE+2];
    int turn;
    int stone_num;
    // struct INT_LIST **score; // 最大数をひっくり返すとき盤面の両端が自分の石になってる -> BOARD_SIZE - 2
    char score[BOARD_SIZE*BOARD_SIZE][BOARD_SIZE+2][BOARD_SIZE+2]; // 棋譜。n手目の盤面をscore[n-1]に保持。
} Game;

extern void game_init(struct Game *game);
extern int opponent(int player);
extern int make_reverse(struct Game *game, int player);
extern void put_stone(struct Game *game, int pos[], int player);
extern int next_turn(struct Game *game);
extern void show_board(Game *game);
extern void prev_board(Game *game, int index, int player);
extern int make_putlist(char reverse[BOARD_SIZE+2][BOARD_SIZE+2], char putlist[][2]);
extern int make_board_to_putlist(char board[BOARD_SIZE+2][BOARD_SIZE+2],
    int player,
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2],
    char putlist[][2]);
extern void put_stone_test(char board[BOARD_SIZE+2][BOARD_SIZE+2],
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2],
    int pos[],
    int player);

#endif