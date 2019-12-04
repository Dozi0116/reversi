#include "const.h"
#include "reversi.h"
#include <stdio.h>
#include "eval.h"

// 評価値の重み
#define SB_INDEX 0
#define PP_INDEX 1
#define OS_INDEX 2
#define FS_INDEX 3
#define MT_INDEX 4
#define WI_INDEX 5

// 通常定数
#define ANY 10

double weights[] = {
    1, // 静的盤面評価
    20, // 着手可能手数
    5, // 開放度
    150, // 確定石
    40, // 山
    80, // ウィング
};

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

double eval_open_space(char board[BOARD_SIZE+2][BOARD_SIZE+2], int pos[2]) {
    // 開放度
    // 置く場所(置いた場所)がどれだけ石に囲まれているかを見る。
    // 最も開放度が低い手がいい手となるが、
    // 今回は有利であればあるほど大きい値を返すことにしているため、
    // -1をかけて返す。

    int score = 0;

    int i;
    for (i = 0;i < DIRECTION_SIZE;i++) {
        if (board[pos[0] + DIRECTION[i][0]][pos[1] + DIRECTION[i][1]] == EMPTY) score++;
    }

    return score;
}

double eval_side_shape(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player) {
    // 辺の評価
    // 確定石の個数と辺の評価(ウィング・山)を判定する

    // 確定石
    // 角から自分の石が連続すれば、それは確定石
    // 自分の石を置いたことで相手の確定石を増やすことはない→相手は考慮しない
    int fixed_stone = 0;
    int tmp;
    int is_chain;

    // 辺の形
    // 一般に、山と呼ばれる形は良型で、ウイングと呼ばれる形は悪型と言われる。
    const char mountain[BOARD_SIZE+2] = {EMPTY, EMPTY, player, player, player, player, player, player, EMPTY, EMPTY};
    const char wing[BOARD_SIZE+2] = {EMPTY, EMPTY, player, player, player, player, player, ANY, EMPTY, EMPTY};
    int is_wing[2] = {FALSE, FALSE};
    int wing_score = 0, mountain_score = 0;
    int wing_multiple = 5, mountain_multiple = 5;

    int x = 0, y = 0;
    // 上辺左→右
    for (x = 1, is_chain = is_wing[0] = TRUE, tmp = 0;x < BOARD_SIZE+1;x++) {
        if (board[1][x] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[1][x] != wing[x] && wing[x] != ANY) is_wing[0] = FALSE;
    }
    // 上辺右→左
    for (x--, is_chain = is_wing[1] = TRUE;x >= 1;x--) {
        if (board[1][x] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[1][x] != wing[BOARD_SIZE+2 - x] && wing[BOARD_SIZE+2 - x] != ANY) is_wing[1] = FALSE;
    }
    if (is_wing[0] && is_wing[1]) {
        // どっちから見てもウイング→山の形
        mountain_score++;
    } else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }
    if (tmp > BOARD_SIZE) {
        // 重複して数えた、つまり、辺全てが自分の色
        tmp = BOARD_SIZE;
    }
    fixed_stone += tmp;

    // 左辺上→下
    for (y = 1, is_chain = is_wing[0] = TRUE, tmp = 0;y < BOARD_SIZE+1;y++) {
        if (board[y][1] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[y][1] != wing[y] && wing[y] != ANY) is_wing[0] = FALSE;
    }
    // 左辺下→上
    for (y--, is_chain = is_wing[1] = TRUE;y >= 1;y--) {
        if (board[y][1] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[y][1] != wing[BOARD_SIZE+2 - y] && wing[BOARD_SIZE+2 - y] != ANY) is_wing[1] = FALSE;
    }
    if (is_wing[0] && is_wing[1]) {
        // どっちから見てもウイング→山の形
        mountain_score++;
    } else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }
    if (tmp > BOARD_SIZE) {
        // 重複して数えた、つまり、辺全てが自分の色
        tmp = BOARD_SIZE;
    }
    fixed_stone += tmp;

    // 右辺上→下
    for (y = 1, is_chain = is_wing[0] = TRUE, tmp = 0;y < BOARD_SIZE+1;y++) {
        if (board[BOARD_SIZE][y] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[BOARD_SIZE][y] != wing[y] && wing[y] != ANY) is_wing[0] = FALSE;
    }
    // 右辺下→上
    for (x--, is_chain = is_wing[1] = TRUE;y >= 1;y--) {
        if (board[BOARD_SIZE][y] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[BOARD_SIZE][y] != wing[BOARD_SIZE+2 - y] && wing[BOARD_SIZE+2 - y] != ANY) is_wing[1] = FALSE;
    }
    if (is_wing[0] && is_wing[1]) {
        // どっちから見てもウイング→山の形
        mountain_score++;
    } else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }
    if (tmp > BOARD_SIZE) {
        // 重複して数えた、つまり、辺全てが自分の色
        tmp = BOARD_SIZE;
    }
    fixed_stone += tmp;

    // 下辺左→右
    for (x = 1, is_chain = is_wing[0] = TRUE, tmp = 0;x < BOARD_SIZE+1;x++) {
        if (board[BOARD_SIZE][x] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[BOARD_SIZE][x] != wing[x] && wing[x] != ANY) is_wing[0] = FALSE;
    }
    // 下辺右→左
    for (x--, is_chain = is_wing[1] = TRUE;x >= 1;x--) {
        if (board[BOARD_SIZE][x] == player && is_chain == TRUE) tmp++;
        else is_chain = FALSE;
        if (board[BOARD_SIZE][x] != wing[BOARD_SIZE+2 - x] && wing[BOARD_SIZE+2 - x] != ANY) is_wing[1] = FALSE;
    }
    if (is_wing[0] && is_wing[1]) {
        // どっちから見てもウイング→山の形
        mountain_score++;
    } else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }
    if (tmp > BOARD_SIZE) {
        // 重複して数えた、つまり、辺全てが自分の色
        tmp = BOARD_SIZE;
    }
    fixed_stone += tmp;

    return fixed_stone * weights[FS_INDEX] + mountain_score * weights[MT_INDEX] - wing_score * weights[WI_INDEX];
}

double eval(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player,
            int pos[2], char putted_board[BOARD_SIZE+2][BOARD_SIZE+2]) {

    // 静的盤面評価
    double sb_score = eval_static_board(putted_board, player) * weights[SB_INDEX];
    // 着手可能手数
    double pp_score = eval_put_position(board, player) * weights[PP_INDEX];
    // 開放度
    double os_score = eval_open_space(board, pos) * weights[OS_INDEX];
    // 辺の評価
    double fs_score = eval_side_shape(putted_board, player);

    return sb_score + pp_score + os_score + fs_score;
}