#include <stdio.h>
#include <math.h>

#include "logic.h"
#include "reversi.h"
#include "const.h"
#include "utility.h"
#include "eval.h"

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
    Node *parent,
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
    char putpos[30][2];
    int index = 0;

    int length = make_putlist(game -> reverse, putpos);

    index = rand() % length; // 置ける場所からランダムに一つ
    pos[0] = putpos[index][0];
    pos[1] = putpos[index][1];
}


struct Node *roulette(Node *nodes[],
    int length) {
    /* 確率に基づいて1つ選出する */

    int i;
    double chance = (double)rand() / RAND_MAX;
    double probability = 0;
    for (i = 0;i < length;i++) {
        probability += nodes[i] -> chance;
        if (probability >= chance) {
            // printf("-> select %d!\n", i);
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
       // printf("  found leaf node!\n");
       return node; 
    }

    int i = 0;
    for (i = 0;i < node -> child_num;i++) {
        // printf("child[%d](%lf) -> %lf, ", i, node -> children[i] -> score, node -> children[i] -> chance);
    }

    return search(roulette(node -> children, node -> child_num));
}

void node_init(Node *node,
    Node *parent,
    char board[BOARD_SIZE+2][BOARD_SIZE+2],
    double score,
    int player) {
    node -> parent = parent;
    
    int i, j;
    for (i = 0;i < 30;i++) {
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
    char putpos[30][2];
    int pos[2];
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2];
    int length = make_board_to_putlist(node -> board, node -> player, reverse, putpos);
    double score;

    if (length == 0) {
        printf("length == 0\n");
        // パスの可能性がある。ゲーム終了ではない可能性
        // 同じ盤面で相手が置けるか見る
        // 置けるならパス→盤面を変えない状態でノードを作成、スコアを伝搬させる。
        // 置けないならゲーム終了→
        
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
            put_stone_test(putted_board, reverse, pos, node -> player);
            node -> children[i] = (Node *)malloc(sizeof(Node));
            score = eval(node -> board, node -> player, pos, putted_board);
            node_init(node -> children[i], node, putted_board, score, opponent(node -> player));
        }

        node -> child_num = i;
    }
}

void chance_update(Node *nodes[], int node_num) {
    /* 温度付きソフトマックス関数
     *                exp(x_i / T)
     *  y_i = -------------------------
     *        sigma_{k=1}^{n}{exp(x_k / T)}
    */
    

    double sum_exp = 0;
    const int T = 1; // ソフトマックス関数の温度。

    int i;
    for (i = 0; i < node_num;i++) {
        sum_exp += exp(nodes[i] -> score / T);
    }

    // ソフトマックス法を使って確率を算出
    for (i = 0;i < node_num;i++) {
        nodes[i] -> chance = exp(nodes[i] -> score / T) / sum_exp;
    }

}

void propagation(Game *game, Node *node) {

    /* 評価値を伝搬させる
     * 評価値 = sum(子ノードの評価点 * ルーレット選択の確率)
     * つまり、子ノードの期待値が親ノードの評価値になる。
    */

    // 伝搬の前に選択確率を更新
    chance_update(node -> children, node -> child_num);

    int i;
    double score = 0;
    for (i = 0; i < node -> child_num;i++) {
        score += node -> children[i] -> score * node -> children[i] -> chance;
    }
    // printf("score update! %lf -> %lf\n", node -> score, -score);
    node -> score = -score;

    if (node -> parent == NULL) {
        // rootノード
        return;
    }

    propagation(game, node -> parent);
}

void bot_softmax(Game *game, int pos[]) {
    const int max_count = 1000;
    int count;

    // 各着手可能位置を調査
    char putpos[30][2];
    int length = make_putlist(game -> reverse, putpos);

    int i, j;
    int index;
    // rootノードを生成しておく
    // rootは今現在の盤面。
    Node root, *target;

    node_init(&root, NULL, game -> board, 0, game -> turn);

    for (i = 0;i < max_count;i++) {
        // 葉ノードまで掘り下げ
        target = search(&root);

        expand(game, target);
        propagation(game, target);
    }

    // 最終的に評価値が最大になったrootの子ノードを選択
    double max_score = root.children[0] -> score;
    pos[0] = putpos[0][0];
    pos[1] = putpos[0][1];
    for (i = 0;i < length;i++) {
        printf("pos(%d, %d) -> score: %f\n", putpos[i][0], putpos[i][1], root.children[i] -> score);
        if (max_score < root.children[i] -> score) {
            max_score = root.children[i] -> score;
            pos[0] = putpos[i][0];
            pos[1] = putpos[i][1];
        }
    }
}