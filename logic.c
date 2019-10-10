#include <stdio.h>
#include <math.h>

#include "logic.h"
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
void node_init(Node *node,
    char board[BOARD_SIZE+2][BOARD_SIZE+2],
    double score,
    int player); // OK
struct Node *roulette(Node *nodes[], int length); // OK
struct Node *search(Node *node); // OK
void expand(Game *game, Node *node); // else部分
void chance_update(Node *nodes[], int node_num); // OK
void propagation(Game *game, Node *node); // OK


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

// int softmax_playout(Game *game, int pos[2]) {

//     int my_player = game -> turn;

//     put_stone(game, pos, game -> turn);
//     int is_game_end = next_turn(game);

//     while (is_game_end != TRUE) {

//         // 置ける手を出す
//         int putpos[60][2];
//         int length = make_putlist(game, putpos);

//         // 盤面を評価
//         int i;
//         int eval_score[length];
//         for (i = 0; i < length;i++) {
//             eval_score[i] = eval(game, game -> turn);
//         }

//         // 評価値を正の数にする
//         int border = min(eval_score);

//         // 評価値をソフトマックス関数にかける
//         int softmax_score[length];
//         softmax_func(eval_score, softmax_score);

//         // softmax_scoreを昇順に並べ替え

//         // ルーレット選択
//         double chance = (double)rand() / RAND_MAX;
//         double probability = 0;
//         for (i = 0;i < length;i++) {
//             probability += softmax_score[i];
//             if (probability >= chance) {

//                 break;
//             }
//         }

//         // put_stone(game, pos, game -> turn);
//         // is_game_end = next_turn(game);
//     }

//     int x, y, point = 0;
//     for (y = 1;y < BOARD_SIZE;y++) {
//         for (x = 1;x < BOARD_SIZE;x++) {
//             if (game -> board[y][x] == my_player) {
//                 point += 1;
//             } else if (game -> board[y][x] == opponent(my_player)) {
//                 point -= 1;
//             }
//         }
//     }

//     if (point == 0) {
//         // 引き分け
//         return 0;
//     } else if (point > 0) {
//         // 勝ち
//         return 1;
//     } else {
//         // 負け
//         return -1;
//     }
// }



struct Node *roulette(Node *nodes[],
    int length) {
    /* 確率に基づいて1つ選出する */

    int i;
    double chance = (double)rand() / RAND_MAX;
    double probability = 0;
    for (i = 0;i < length;i++) {
        probability += nodes[i] -> chance;
        if (probability >= chance) {
            return nodes[i];
        }
    }

}

struct Node *search(Node *node) {
    /*
    *  末端ノードor新規子ノードに来るまで掘り下げる
    *  これ以上打つべき点が無い→末端ノード
    */

    if (node -> children[0] == NULL) {
       // 子ノードがいない
       return node; 
    }

    return search(roulette(node -> children, node -> child_num));
}

void node_init(Node *node,
    char board[BOARD_SIZE+2][BOARD_SIZE+2],
    double score,
    int player) {
    node -> parent = NULL;
    
    int i, j;
    for (i = 0;i < 60;i++) {
        node -> children[i] = NULL;
    }

    for (i = 0;i < BOARD_SIZE+2;i++) {
        for (j = 0;j < BOARD_SIZE+2;j++) {
            node -> board[i][j] = board[i][j];
        }
    }

    node -> child_num = 0;

    node -> score = score;

    node -> player = player;
}

void expand(Game *game, Node *node) {
    /* 末端ノードを展開する */

    // 置ける場所を求める
    int i, j, k;
    char putted_board[BOARD_SIZE+2][BOARD_SIZE+2];
    char putpos[60][2];
    int pos[2];
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2];
    int length = make_board_to_putlist(node -> board, node -> player, reverse, putpos);

    if (length == 0) {
        // ゲーム終了　これより下のノードはない。
        // 勝利しているならボーナスポイントを追加したい
    } else {
        for (i = 0;i < length;i++) {
            // boardをコピー
            for (j = 0;j < BOARD_SIZE+2;j++) {
                for (k = 0;k < BOARD_SIZE+2;k++) {
                    putted_board[j][k] = node -> board[j][k];
                }
            }
            pos[0] = putpos[i][0];
            pos[1] = putpos[i][1];
            put_stone_test(putted_board, pos, reverse, node -> player);
            node_init(node -> children[i], putted_board, eval(putted_board), opponent(node -> player));
        }

        node -> child_num = i;
    }
}

void chance_update(Node *nodes[], int node_num) {
    /* ソフトマックス関数
     *                exp(x_i)
     *  y_i = -------------------------
     *        sigma_{k=1}^{n}{exp(x_k)}
    */
    

    double sum_exp = 0;

    int i;
    for (i = 0; i < node_num;i++) {
        sum_exp += exp(nodes[i] -> score);
    }

    // ソフトマックス法を使って確率を算出
    for (i = 0;i < node_num;i++) {
        nodes[i] -> chance = exp(nodes[i] -> score) / sum_exp;
    }

}

void propagation(Game *game, Node *node) {

    /* 評価値を伝搬させる
     * 評価値 = sum(子ノードの評価点 * ルーレット選択の確率)
    */

    // 伝搬の前に選択確率を更新
    chance_update(node -> children, node -> child_num);

    int i;
    double score = 0;
    for (i = 0; i < node -> child_num;i++) {
        score += node -> children[i] -> score * node -> children[i] -> chance;
    }

    if (node -> parent == NULL) {
        // rootノード
        return;
    }

    propagation(game, node -> parent);
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
    int i, j;
    int index;
    // rootノードを生成しておく
    // rootは今現在の盤面。
    Node root, *target;

    node_init(&root, game -> board, eval(game -> board), opponent(game -> turn));

    for (i = 0;i < max_count;i++) {
        // 子ノードまで掘り下げ
        target = search(&root);

        char putpos[60][2];
        char reverse[BOARD_SIZE+2][BOARD_SIZE+2];

        index = make_board_to_putlist(root.board, root.player, reverse, putpos);

        if (index == 0) {
            // 勝敗がわかっているノードの末端

            // 勝ちなら評価点をちょっとあげたい

        } else {
            // 勝敗がわかっていないノードの末端
            // ノードを生成
            expand(game, target);

            // スコアを伝搬させていく
            propagation(game, target -> parent);
        }
    }
    // int i;
    // for (i = 0;i < length;i++) {
    //     win_point[i] = 0;
    //     for (count = 0;count < max_count;count++) {
    //         win_point[i] += softmax_playout(game, game -> turn); // TODO
    //     }

    //     // 盤面を巻き戻す
    //     prev_board(game, origin_stone_num, game -> turn);
    // }

    double max_score = root.children[0] -> score;
    pos[0] = putpos[0][0];
    pos[1] = putpos[0][1];
    for (i = 0;i < length;i++) {
        if (max_score < root.children[i] -> score) {
            max_score = root.children[i] -> score;
            pos[0] = putpos[i][0];
            pos[1] = putpos[i][1];
        }
    }
}