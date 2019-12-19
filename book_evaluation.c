/*
『リバーシのアルゴリズム』にかかれている
評価関数(c++)版を自分の盤面に合うように実装した。
*/

#include "const.h"
#include "reversi.h"
#include <stdio.h>
#include "book_evaluation.h"

#define ANY 10

typedef struct {
    double corner_stone_weight;
    double fixed_stone_weight;
    double open_weight;
    double putpos_weight;
    double mountain_weight;
    double wing_weight;
    double c_weight;
    double x_weight;
} weights_t;

typedef struct {
    int corner_stone_num;
    int fixed_stone_num;
    int open_num;
    int putpos_num;
    int mountain_num;
    int wing_num;
    int c_num;
    int x_num;
} score_t;

// 本を参考にした数値
// 0は係数が無いところ
// 評価スコアがインフレするため、全部に0.1をかけた
weights_t weights_s = {
    0, // corner
    10.1, // fixed(stable)
    -1.3, // open(liberty)
    6.7, // putpos(mobility)
    0, // mountain
    -30.8, // wing(wing)
    -55.2, // c(Cmove)
    -44.9  // x(Xmove)
};

//序盤・中盤・終盤で評価方法を変える


/*
辺の評価
確定石の個数と辺の評価(ウィング・山・C打ち)を判定する
*/
void eval_edge(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player, score_t *score) {
    score -> fixed_stone_num = 0;
    score -> mountain_num = 0;
    score -> wing_num = 0;
    score -> c_num = 0;

    // 確定石
    // 角から自分の石が連続すれば、それは確定石
    int fixed_stone = 0;
    int tmp;
    int is_chain;

    // 辺の形
    // ウイングと呼ばれる形は悪型と言われる。
    const char wing[BOARD_SIZE+2] = {EMPTY, EMPTY, player, player, player, player, player, ANY, EMPTY, EMPTY};
    // 山と呼ばれる形は良型と言われる。(変数の必要が無いためコメントアウトした)
    // const char mountain[BOARD_SIZE+2] = {EMPTY, EMPTY, player, player, player, player, player, player, EMPTY, EMPTY};

    int is_wing[2] = {FALSE, FALSE};
    int wing_score = 0, mountain_score = 0, c_score = 0;
    int wing_multiple = 5, mountain_multiple = 5;

    int x, y;
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

    // どっちから見てもウイング→山の形
    if (is_wing[0] && is_wing[1]) mountain_score++;
    else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }

    // ウイングになっていないかつCが自分の色→C打ち
    // 山でもウイングでもない かつ 角に置かれていない かつ Cが自分の色→
    if (!is_wing[0] && !is_wing[1]) {
        if (board[1][1] == EMPTY && board[1][2] == player) c_score++;
        if (board[1][BOARD_SIZE] == EMPTY && board[1][BOARD_SIZE-1] == player) c_score++;
    }

    // 重複して数えた→辺全てが自分の色
    if (tmp > BOARD_SIZE) tmp = BOARD_SIZE;
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

    if (is_wing[0] && is_wing[1]) mountain_score++;
    else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }

    if (!is_wing[0] && !is_wing[1]) {
        if (board[1][1] == EMPTY && board[2][1] == player) c_score++;
        if (board[BOARD_SIZE][1] == EMPTY && board[BOARD_SIZE-1][1] == player) c_score++;
    }

    if (tmp > BOARD_SIZE) tmp = BOARD_SIZE;
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

    if (is_wing[0] && is_wing[1]) mountain_score++;
    else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }

    if (!is_wing[0] && !is_wing[1]) {
        if (board[BOARD_SIZE][1] == EMPTY && board[BOARD_SIZE][2] == player) c_score++;
        if (board[BOARD_SIZE][BOARD_SIZE] == EMPTY && board[BOARD_SIZE][BOARD_SIZE-1] == player) c_score++;
    }

    if (tmp > BOARD_SIZE) tmp = BOARD_SIZE;
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

    if (is_wing[0] && is_wing[1]) mountain_score++;
    else {
        if (is_wing[0]) wing_score++;
        if (is_wing[1]) wing_score++;
    }

    if (!is_wing[0] && !is_wing[1]) {
        if (board[1][BOARD_SIZE] == EMPTY && board[2][BOARD_SIZE] == player) c_score++;
        if (board[BOARD_SIZE][BOARD_SIZE] == EMPTY && board[BOARD_SIZE-1][BOARD_SIZE] == player) c_score++;
    }

    if (tmp > BOARD_SIZE) tmp = BOARD_SIZE;
    fixed_stone += tmp;


    score -> fixed_stone_num = fixed_stone;
    score -> mountain_num = mountain_score;
    score -> wing_num = wing_score;
    score -> c_num = c_score;
}


/*
角の評価
各角の石とX打ちを数え上げる。
また、重複して数えていた角の確定石を減らす。
*/
void eval_corner(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player, score_t *score) {


    score -> corner_stone_num = 0;
    score -> x_num = 0;

    // 左上
    if (board[1][1] == player) {
        score -> fixed_stone_num--;
        (score -> corner_stone_num)++;
    }
    else if (board[1][1] == EMPTY && board[2][2] == player) (score -> x_num)++;

    // 右上
    if (board[1][BOARD_SIZE] == player) {
        score -> fixed_stone_num--;
        (score -> corner_stone_num)++;
    }
    else if (board[1][BOARD_SIZE] == EMPTY && board[2][BOARD_SIZE-1] == player) (score -> x_num)++;

    // 左下
    if (board[BOARD_SIZE][1] == player) {
        score -> fixed_stone_num--;
        (score -> corner_stone_num)++;
    }
    else if (board[BOARD_SIZE][1] == EMPTY && board[BOARD_SIZE-1][2] == player) (score -> x_num)++;

    // 右下
    if (board[BOARD_SIZE][BOARD_SIZE] == player) {
        score -> fixed_stone_num--;
        (score -> corner_stone_num)++;
    }
    else if (board[BOARD_SIZE][BOARD_SIZE] == EMPTY && board[BOARD_SIZE-1][BOARD_SIZE-1] == player) (score -> x_num)++;
}


/*
開放度の評価
自分の石の周りを見る
*/
void eval_open(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player, score_t *score) {
    score -> open_num = 0;

    int x, y, open_score, i;
    for (y = 1;y < BOARD_SIZE+1;y++) {
        for (x = 1;x < BOARD_SIZE+1;x++) {
            if (board[y][x] != player) continue;

            // 周りの石を見る
            open_score = 0;
            if (x == 1 || x == BOARD_SIZE || y == 1 || y == BOARD_SIZE) open_score -= 3;
            if (
                (y == 1 && x == 1) || (y == 1 && x == BOARD_SIZE) ||
                (y == BOARD_SIZE && x == 1) || (y == BOARD_SIZE && x == BOARD_SIZE)
            ) open_score -= 2;

            for (i = 0;i < DIRECTION_SIZE;i++) {
                if (board[y + DIRECTION[i][0]][x + DIRECTION[i][1]] == EMPTY) open_score++;
            }

            score -> open_num += open_score;
        }
    }
}


/*
着手可能数の評価
*/
int eval_putpos(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player, score_t *score) {
    char dummy1[BOARD_SIZE+2][BOARD_SIZE+2];
    char dummy2[30][2];
    score -> putpos_num = make_board_to_putlist(board, player, dummy1, dummy2);
}


/*
盤面を評価して返す関数。
board: 置いたあとの盤面
player: 置いた人

返り値: 評価スコア
*/
double evaluation(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player) {
    score_t my_score = {0.0};
    score_t opponent_score = {0.0};
    double result = 0;

    eval_edge(board, player, &my_score);
    eval_corner(board, player, &my_score);
    eval_open(board, player, &my_score);
    eval_putpos(board, player, &my_score);

    // printf("weights\n%5lf, %5lf, %5lf, %5lf, %5lf, %5lf, %5lf, %5lf\n\n", \
    //     weights_s.fixed_stone_weight,
    //     weights_s.corner_stone_weight,
    //     weights_s.open_weight,
    //     weights_s.putpos_weight,
    //     weights_s.mountain_weight,
    //     weights_s.wing_weight,
    //     weights_s.c_weight,
    //     weights_s.x_weight);

    result += ( \
        (double)my_score.fixed_stone_num * weights_s.fixed_stone_weight + \
        (double)my_score.corner_stone_num * weights_s.corner_stone_weight + \
        (double)my_score.open_num * weights_s.open_weight + \
        (double)my_score.putpos_num * weights_s.putpos_weight + \
        (double)my_score.mountain_num * weights_s.mountain_weight + \
        (double)my_score.wing_num * weights_s.wing_weight + \
        (double)my_score.c_num * weights_s.c_weight + \
        (double)my_score.x_num * weights_s.x_weight);

    // printf("my score\n%5lf, %5lf, %5lf, %5lf, %5lf, %5lf, %5lf, %5lf\n\n", \
    //     my_score.fixed_stone_num * weights_s.fixed_stone_weight,
    //     my_score.corner_stone_num * weights_s.corner_stone_weight,
    //     my_score.open_num * weights_s.open_weight,
    //     my_score.putpos_num * weights_s.putpos_weight,
    //     my_score.mountain_num * weights_s.mountain_weight,
    //     my_score.wing_num * weights_s.wing_weight,
    //     my_score.c_num * weights_s.c_weight,
    //     my_score.x_num * weights_s.x_weight);


    eval_edge(board, opponent(player), &opponent_score);
    eval_corner(board, opponent(player), &opponent_score);
    eval_open(board, opponent(player), &opponent_score);
    // 原本にはなかったが、相手のスコア = -(自分のスコア)とするため、追加した。
    eval_putpos(board, player, &opponent_score);


    result -= ( \
        (double)opponent_score.fixed_stone_num * weights_s.fixed_stone_weight + \
        (double)opponent_score.corner_stone_num * weights_s.corner_stone_weight + \
        (double)opponent_score.open_num * weights_s.open_weight + \
        (double)opponent_score.putpos_num * weights_s.putpos_weight + \
        (double)opponent_score.mountain_num * weights_s.mountain_weight + \
        (double)opponent_score.wing_num * weights_s.wing_weight + \
        (double)opponent_score.c_num * weights_s.c_weight + \
        (double)opponent_score.x_num * weights_s.x_weight);

    // printf("opponent score\n%5lf, %5lf, %5lf, %5lf, %5lf, %5lf, %5lf, %5lf\n\n", \
    //     opponent_score.fixed_stone_num * weights_s.fixed_stone_weight,
    //     opponent_score.corner_stone_num * weights_s.corner_stone_weight,
    //     opponent_score.open_num * weights_s.open_weight,
    //     opponent_score.putpos_num * weights_s.putpos_weight,
    //     opponent_score.mountain_num * weights_s.mountain_weight,
    //     opponent_score.wing_num * weights_s.wing_weight,
    //     opponent_score.c_num * weights_s.c_weight,
    //     opponent_score.x_num * weights_s.x_weight);

    // printf("eval score -> %5lf\n", result);

    return result;
}