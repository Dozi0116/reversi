#include "const.h"
#include "reversi.h"
#include <stdio.h>
#include "eval.h"

#define SB_WEIGHT 1
#define PP_WEIGHT 1
#define OS_WEIGHT 1

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

    static const int board_score[BOARD_SIZE+2][BOARD_SIZE+2] = {
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
            } else if (board[i][j] == opponent(player)) {
                score -= board_score[i][j];
            }
        }
    }

    // printf("eval_static_board -> %f\n", score);
    return score;
}

double eval_put_position(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player) {
    // 着手可能手数
    // 自分が置ける手数 - 相手が置ける手数
    // https://github.com/nehori/c-othello/blob/master/source/eval.c
    // 上記のURLの評価関数を参考にした。
    // http://sealsoft.jp/thell/algorithm.html
    // ここを参照すると5〜16手目まではとても寄与するが、それ以外はほとんど寄与しない
    // 着手可能数の重みは手数によって変更する必要がありそう

    char reverse[BOARD_SIZE+2][BOARD_SIZE+2];
    char putlist[30][2];
    int i, length;

    int my_score = make_board_to_putlist(board, player, reverse, putlist);
    for (i = 0, length = my_score;i < length;i++) {
        if ((putlist[i][0] == 2 && putlist[i][1] == 2)
         || (putlist[i][0] == 2 && putlist[i][1] == 7)
         || (putlist[i][0] == 7 && putlist[i][1] == 2)
         || (putlist[i][0] == 7 && putlist[i][1] == 7)) {
                my_score--; // x打ち(角の内側のところ)は着手可能であってもアウト
        }
    }

    int opponent_score = make_board_to_putlist(board, opponent(player), reverse, putlist);
    for (i = 0, length = opponent_score;i < length;i++) {
        if ((putlist[i][0] == 2 && putlist[i][1] == 2)
         || (putlist[i][0] == 2 && putlist[i][1] == 7)
         || (putlist[i][0] == 7 && putlist[i][1] == 2)
         || (putlist[i][0] == 7 && putlist[i][1] == 7)) {
                opponent_score--; // x打ち(角の内側のところ)は着手可能であってもアウト
        }
    }

    return my_score - opponent_score;
}


double eval(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player,
            int pos[2], char putted_board[BOARD_SIZE+2][BOARD_SIZE+2]) {
    // 静的盤面評価
    double sb_score = eval_static_board(putted_board, player) * SB_WEIGHT;
    // 着手可能手数
    double pp_score = eval_put_position(board, player) * PP_WEIGHT;

    return sb_score + pp_score;
}