#include "logic.h"
#include <stdio.h>
#include "reversi.h"
#include "const.h"
#include "utility.h"

/*
    各種入力用の関数一覧。関数名はbot_xxxとする(playerのみ例外)。
    引数
        struct Game * : 盤面などの情報があるオブジェクト
        int pos [] : 置く場所を格納する用の配列。pos[0]で行、pos[1]で列を指定する。

    注意(責任の位置が違うかもしれない)
        posに格納される値は「ルール上石が置ける場所」に限る。
        置けない場所を指定した場合の動作は保証しない。
*/

int softmax_playout(struct Game *game, int pos[2]);


/*
    人間がキーボードから入力を行う用のロジック
*/
void player(Game *game, int pos[]) {
    char command[3];
    printf("put pos (ex. 1A) -> ");
    while (TRUE) {
        input(command);

        pos[0] = c2i(command[0]);
        pos[1] = c2i(command[1]) + 1; // 文字「A」がインデックス「1」に対応。

        if (game -> reverse[pos[0]][pos[1]] != 0) {
            break;
        } else {
            printf("position error. Please try again.\n");
        }
    }
}

void bot_random(Game *game, int pos[]) {
    char putpos[60][2];
    int index = 0;

    int length = make_putlist(game -> reverse, putpos);

    index = rand() % length; // 置ける場所からランダムに一つ
    pos[0] = putpos[index][0];
    pos[1] = putpos[index][1];
}

int softmax_playout(Game *game, int pos[2]) {

    int my_player = game -> turn;

    put_stone(game, pos, game -> turn);
    int is_game_end = next_turn(game);

    while (is_game_end != TRUE) {

        // 置ける手を出す
        int putpos[60][2];
        int length = make_putlist(game, putpos);

        // 盤面を評価
        int i;
        int eval_score[length];
        for (i = 0; i < length;i++) {
            eval_score[i] = eval(game, game -> turn);
        }

        // 評価値を正の数にする
        int border = min(eval_score);

        // 評価値をソフトマックス関数にかける
        int softmax_score[length];
        softmax_func(eval_score, softmax_score);

        // softmax_scoreを昇順に並べ替え

        // ルーレット選択
        double chance = (double)rand() / RAND_MAX;
        double probability = 0;
        for (i = 0;i < length;i++) {
            probability += softmax_score[i];
            if (probability >= chance) {

                break;
            }
        }

        // put_stone(game, pos, game -> turn);
        // is_game_end = next_turn(game);
    }

    int x, y, point = 0;
    for (y = 1;y < BOARD_SIZE;y++) {
        for (x = 1;x < BOARD_SIZE;x++) {
            if (game -> board[y][x] == my_player) {
                point += 1;
            } else if (game -> board[y][x] == opponent(my_player)) {
                point -= 1;
            }
        }
    }

    if (point == 0) {
        // 引き分け
        return 0;
    } else if (point > 0) {
        // 勝ち
        return 1;
    } else {
        // 負け
        return -1;
    }
}

void bot_softmax(Game *game, int pos[]) {
    const int max_count = 500;
    int count;
    const int origin_stone_num = game -> stone_num;

    // 各着手可能位置を調査
    char putpos[60][2];
    int length = make_putlist(game -> reverse, putpos);

    int win_point[length];

    // プレイアウトをmax_count回行う
    int i;
    for (i = 0;i < length;i++) {
        win_point[i] = 0;
        for (count = 0;count < max_count;count++) {
            win_point[i] += softmax_playout(game, game -> turn); // TODO
        }

        // 盤面を巻き戻す
        prev_board(game, origin_stone_num, game -> turn);
    }

    int max_score = win_point[0];
    int pos[0] = putpos[0][0];
    int pos[1] = putpos[0][1];
    for (i = 1;i < length;i++) {
        if (win_point[i] > max_score) {
            max_score = win_point[i];
            pos[0] = putpos[i][0];
            pos[1] = putpos[i][1];
        }
    }
}