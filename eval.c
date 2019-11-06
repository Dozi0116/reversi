#include "const.h"
#include "reversi.h"
#include <stdio.h>

/*
    各種評価用の関数一覧。関数名はeval_xxxとする(evalのみ例外)。
    引数
        int board[BOARD_SIZE+2][BOARD_SIZE+2] : 盤面
        int player : 基準にする側のplayer。通常は打った側を入れておく。
            →(playerがここに打って点差がどう変わったというのがわかる)

    返り値
        double 評価点。

    注意(責任の位置が違うかもしれない)
        評価点は引数で渡したplayerを基準にする。互角は0点、戦況に差が出れば出るほど点差が大きくなる。
*/

double eval_static_board(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player) {
    // 静的盤面評価
    // 予め、マスごとに決められた点数を加算する。
    // (先輩のプログラムは自分の石だけ見ていたから、今回は自分の石だけ考慮)
    const int board_score[BOARD_SIZE+2][BOARD_SIZE+2] = {
        {0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
        {0, 100, -50,  10,   0,   0,  10, -50, 100, 0},
        {0, -50, -70,  -5, -10, -10,  -5, -70, -50, 0},
        {0,  10,  -5, -10,  -5,  -5, -10,  -5,  10, 0},
        {0,   0, -10,  -5,   0,   0,  -5, -10,   0, 0},
        {0,   0, -10,  -5,   0,   0,  -5, -10,   0, 0},
        {0,  10,  -5, -10,  -5,  -5, -10,  -5,  10, 0},
        {0, -50, -70,  -5, -10, -10,  -5, -70, -50, 0},
        {0, 100, -50,  10,   0,   0,  10, -50, 100, 0},
        {0,   0,   0,   0,   0,   0,   0,   0,   0, 0}
    };

    double score = 0;

    int i, j;
    for (i = 1;i < BOARD_SIZE+1;i++) { 
        for (j = 1;j < BOARD_SIZE+1;j++) {
            if (board[i][j] == player) {
                score += board_score[i][j];
            }
        }
    }

    // printf("eval_static_board -> %f\n", score);
    return score;
}


double eval(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player) {
    double esb_score = eval_static_board(board, player);
    printf("return score -> %f\n", esb_score);
    return esb_score;
}