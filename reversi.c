#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
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

    for (y = 0; y < BOARD_SIZE+2;y++){
        for (x = 0;x < BOARD_SIZE+2;x++){
            game -> board[y][x] = EMPTY;
            game -> reverse[y][x] = 0;
        }
    }

    game -> turn = BLACK;
    game -> stone_num = 0;

    /*
    scoreのとり方を変更
    char score[64][8][8]として、盤面の情報を毎ターン保持しておく方向に変更。
    */
    
    int start_pos[4][2] = {
        {4, 4},
        {5, 5},
        {4, 5},
        {5, 4}
    };

    put_stone(game, start_pos[0], game -> turn);
    put_stone(game, start_pos[1], game -> turn);
    put_stone(game, start_pos[2], opponent(game -> turn));
    put_stone(game, start_pos[3], opponent(game -> turn));

    make_reverse(game, game -> turn);
}

int opponent(int player) {
    // BLACKならWHITEを、WHITEならBLACKを返す。
    return player == BLACK ? WHITE : BLACK;
}

int make_reverse(struct Game *game,
                  int player) {
    /*
    playerとboardからひっくり返せる場所を見つけて返す。
    返り値は置けるかどうか。TRUEで置ける。
    */

   int y, x;
   int dy, dx;
   int i;
   int is_put = FALSE;

    for (y = 1;y <= BOARD_SIZE;y++) {
        for (x = 1;x <= BOARD_SIZE;x++) {
            game -> reverse[y][x] = 0; // 初期化
            if (game -> board[y][x] != EMPTY) continue;
            for (i = 0;i < 8;i++) {
                dy = y + DIRECTION[i][0];
                dx = x + DIRECTION[i][1];
                if (game -> board[dy][dx] == opponent(player)) {
                    do {
                        dy += DIRECTION[i][0];
                        dx += DIRECTION[i][1];
                    } while (game -> board[dy][dx] == opponent(player));
                    
                    if (game -> board[dy][dx] == player) {
                        is_put = TRUE;
                        game -> reverse[y][x] |= FLAG[i];
                    }
                }
            }
        }
    }

    return is_put;
}

/*
* TODO restore関数
* void restore(Game *game, int score_index);
*/


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
    score_index++;

    for (i = 0; i < DIRECTION_SIZE;i++) {
        if ((game -> reverse[y][x] & FLAG[i]) != 0) {
            // その方向に返すことができる
            dy = y + DIRECTION[i][0];
            dx = x + DIRECTION[i][1];
            do {
                game -> board[dy][dx] = player;
                score_index++;
                dy += DIRECTION[i][0];
                dx += DIRECTION[i][1];
            } while (game -> board[dy][dx] == opponent(player));           
        }
    }

    // 棋譜更新
    int j;
    for (i = 0;i < BOARD_SIZE+2;i++) {
        for(j = 0;j < BOARD_SIZE+2;j++) {
            game -> score[game -> stone_num][i][j] = game -> board[i][j];
        }
    }
    (game -> stone_num)++;
}


int next_turn(Game *game) {
    // game構造体を次のターンへ進める。
    // 返り値はゲームが終了するかどうか。

    int is_game_end = FALSE;
    int is_put;
    game -> turn = opponent(game -> turn);
    is_put = make_reverse(game, game -> turn);

    if (is_put == FALSE) {
        // パス
        game -> turn = opponent(game -> turn);
        is_put = make_reverse(game, game -> turn);

        if (is_put == FALSE) {
            is_game_end = TRUE;
        }
    }

    return is_game_end;
}

void show_board(Game *game) {
    int i, j;
    int w_count = 0;
    int b_count = 0;

    printf("    A B C D E F G H\n");
    for (i = 1;i < BOARD_SIZE+1;i++) {
        printf("%d   ", i);
        for (j = 1;j < BOARD_SIZE+1;j++) {
            switch ((game -> board)[i][j]) {
                case BLACK:
                    b_count++;
                    printf("o ");
                    break;
                case WHITE:
                    w_count++;
                    printf("x ");
                    break;
                case EMPTY:
                    if((game -> reverse)[i][j] != 0) {
                        printf(". ");
                    } else {
                        printf("_ ");
                    }
                    break;
                default:
                    printf("error");
            }
        }
        printf("\n");
    }
    
    printf("\nstatus\n");
    printf("next turn -> ");
    switch (game -> turn) {
        case BLACK:
            printf("o\n");
            break;
        case WHITE:
            printf("x\n");
            break;
        default:
            printf("error (%d)\n", game -> turn);
            break;
    }
    printf("o -> %d, x -> %d\n", b_count, w_count);

    printf("\n\n\n");
}

void prev_board(Game *game,
                int index,
                int player) {
    int x, y;

    for (y = 1;y < BOARD_SIZE+1;y++) {
        for (x = 1;x < BOARD_SIZE+1;x++) {
            game -> board[y][x] = game -> score[index-1][y][x];
        }
    }

    make_reverse(game, player);
}

int make_putlist(char reverse[BOARD_SIZE+2][BOARD_SIZE+2], char putlist[][2]) {
    int length = 0;
    int x, y;
    for (y = 1;y < BOARD_SIZE+1;y++) {
        for (x = 1;x < BOARD_SIZE+1;x++) {
            if (reverse[y][x] != 0) {
                putlist[length][0] = y;
                putlist[length][1] = x;
                length++;
            }
        }
    }

    return length;
}

int make_board_to_putlist(char board[BOARD_SIZE+2][BOARD_SIZE+2],
                 int player,
                 char reverse[BOARD_SIZE+2][BOARD_SIZE+2],
                 char putlist[][2]) {
    /*
    playerとboardからひっくり返せる場所を見つけて返す。
    */

   int y, x;
   int dy, dx;
   int i;
   int index = 0;

    for (y = 1;y <= BOARD_SIZE;y++) {
        for (x = 1;x <= BOARD_SIZE;x++) {
            reverse[y][x] = 0; // 初期化
            if (board[y][x] != EMPTY) continue;
            for (i = 0;i < 8;i++) {
                dy = y + DIRECTION[i][0];
                dx = x + DIRECTION[i][1];
                if (board[dy][dx] == opponent(player)) {
                    do {
                        dy += DIRECTION[i][0];
                        dx += DIRECTION[i][1];
                    } while (board[dy][dx] == opponent(player));
                    
                    if (board[dy][dx] == player) {
                        reverse[y][x] |= FLAG[i];
                    }
                }
            }

            if (reverse[y][x] != 0) {
                putlist[index][0] = y;
                putlist[index][1] = x;
                index++;
            }
        }
    }

    return index;
}

void put_stone_test(char board[BOARD_SIZE+2][BOARD_SIZE+2],
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2],
    int pos[],
    int player) {
    /*
    boardのposにplayerの石を置く。
    */

   int y = pos[0], x = pos[1];
   int dy, dx;
   int i;
   int score_index = 0;

   board[y][x] = player;
   score_index++;

   for (i = 0; i < DIRECTION_SIZE;i++) {
       if ((reverse[y][x] & FLAG[i]) != 0) {
           // その方向に返すことができる
            dy = y + DIRECTION[i][0];
            dx = x + DIRECTION[i][1];
            do {
                board[dy][dx] = player;
                score_index++;
                dy += DIRECTION[i][0];
                dx += DIRECTION[i][1];
            } while (board[dy][dx] == opponent(player));           
       }
   }
}

void board_print(char board[BOARD_SIZE+2][BOARD_SIZE+2]) {
    int i, j;
    printf("    A B C D E F G H\n");
    for (i = 1;i < BOARD_SIZE+1;i++) {
        printf("%d   ", i);
        for (j = 1;j < BOARD_SIZE+1;j++) {
            switch (board[i][j]) {
                case BLACK:
                    printf("o ");
                    break;
                case WHITE:
                    printf("x ");
                    break;
                case EMPTY:
                    printf("_ ");
                    break;
                default:
                    printf("e ");
            }
        }
        printf("\n");
    }
}

/*
ボードの状態を数え、playerが勝っているなら1, 負けているなら-1, 同点なら0を返す。
*/
int count_board(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player) {
    int x, y;
    int stone = 0;
    for (y = 0;y <= BOARD_SIZE;y++) {
        for (x = 0;x <= BOARD_SIZE;x++) {
            if (board[y][x] == player) stone++;
            else if (board[y][x] == opponent(player)) stone--;
        }
    }

    if (stone > 0) return 1;
    if (stone < 0) return -1;
    if (stone == 0) return 0;
}