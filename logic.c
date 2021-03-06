#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "logic.h"
#include "reversi.h"
#include "const.h"
#include "utility.h"
// #include "eval.h"
#include "book_evaluation.h"

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
    int player);
struct Node *roulette(Node *nodes[], int length);
struct Node *search(Node *node);
void expand(Game *game, Node *node);
void chance_update(Node *nodes[], int node_num);
void propagation(Game *game, Node *node);

int match = 0;

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


/*
とりあえず適当に打つロジック
*/
void bot_random(Game *game, int pos[]) {
    char putpos[30][2];
    int index = 0;

    int length = make_putlist(game -> reverse, putpos);

    index = rand() % length; // 置ける場所からランダムに一つ
    pos[0] = putpos[index][0];
    pos[1] = putpos[index][1];
}


/*
確率に基づいて1つ選出する
*/
struct Node *roulette(
    Node *nodes[],
    int length) {

    int i;
    double chance = (double)rand() / RAND_MAX;
    double probability = 0;
    for (i = 0;i < length;i++) {
        probability += nodes[i] -> chance;
        if (probability >= chance) {
            return nodes[i];
        }
    }

    // 確率を足し上げていった結果、誤差で100%にわずかに満たず、たまたま引いた乱数がその僅かを指していた。
    // 誤差なので、最後の要素を返す
    return nodes[length-1];

}


/*
末端ノードor新規子ノードに来るまで掘り下げる
これ以上打つべき点が無い→末端ノード
*/
struct Node *search(Node *node) {

    if (node -> children[0] == NULL) {
       return node; 
    }

    Node *target = roulette(node -> children, node -> child_num);

    return search(target);
}

/*
ノードの初期化を行う関数。
*/
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
    node -> chance = -1;
}


/*
末端ノードを展開する関数。
*/
void expand(Game *game, Node *node) {
    // 置ける場所を求める
    int i, j, k;
    char putted_board[BOARD_SIZE+2][BOARD_SIZE+2] = {{0}};
    char putpos[30][2] = {{0}};
    int pos[2];
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2] = {0};
    int length = make_board_to_putlist(node -> board, node -> player, reverse, putpos);
    double score = 0.0;

    if (length == 0) {
        // パスの可能性がある。ゲーム終了ではない可能性
        // 同じ盤面で相手が置けるか見る
        length = make_board_to_putlist(node -> board, opponent(node -> player), reverse, putpos);
        if (length == 0) {
            // 決着ノード
            // 勝敗に応じて大きい点数を与える。
            // printf("game end node\n");

            /*
            注意！！
            ここのプログラムは冗長にかかれており、動作が遅くなる原因になっている。
            count_board関数は動作が重く、何回も実行しては1秒単位で動作に遅れが生じる。

            以下の188行目までをif (node -> chance == -1) {} で囲い、node_init関数のnode -> chance = -1の行のコメントアウトを外す。
            次にif文の中身をnode -> score = に変更する。
            必要に応じて、10/-10ではなく、700/-700となるべく大きな値を入れる。
            そして、193行目を消すことで、最適化が完了すると思われる。
            */

            if (node -> chance == -1) {
                int result = count_board(node -> board, game -> turn);

                // 決着した場合、特殊なスコアを付けているが、これは本来は良くない可能性…？
                // 振り切りスコア(-730など)をいれるのが良かったか？
                if (result > 0) {
                    // win
                    score = MAX_SCORE_SOFTMAX;
                } else if (result < 0) {
                    // lose
                    score = MIN_SCORE_SOFTMAX;
                } else {
                    // draw
                    score = 0;
                }

                node -> children[0] = (Node *)malloc(sizeof(Node));
                node_init(node -> children[0], node, node -> board, -score, opponent(node -> player));
            }
            // node -> child_num = 1;

            return;
        } else {
            // パスノード
            // 盤面を変えない状態でノードを作成。
            // printf("pass node\n");
            node -> children[0] = (Node *)malloc(sizeof(Node));
            node_init(node -> children[0], node, node -> board, -(node -> score), opponent(node -> player));
            node -> child_num = 1;
        }
    } else {
        // 通常ノード
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
            score = evaluation(putted_board, node -> player);
            node_init(node -> children[i], node, putted_board, score, opponent(node -> player));
        }

        // printf("end expand\n");

        node -> child_num = length;
    }
}


double softmax_T = 10;

/*
温度付きソフトマックス関数
               exp(x_i / T)
y_i = -------------------------
      sigma_{k=1}^{n}{exp(x_k / T)}

注意: exp(730)あたりを超えると、計算できなくなってしまうので注意！！
対策: 温度を用いて730以下にする(今これ)、上限を設ける、730を超えないような評価関数を作成するetc...
*/
void chance_update(Node *nodes[], int node_num) {
    double sum_exp = 0;

    int i;
    // printf("softmax_T -> %lf\n", softmax_T);
    for (i = 0; i < node_num;i++) {
        if (nodes[i] -> score / softmax_T > 700) {
            // printf("over 700!!!!\n");
            // printf("\t score -> %lf\n", -nodes[i] -> score);
            nodes[i] -> score = 700;
        }
        if (-(nodes[i] -> score) / softmax_T > 700) {
            // printf("under 700!!!\n");
            // printf("\t score -> %lf\n", -(nodes[i] -> score));
            nodes[i] -> score = -700;
        }

        sum_exp += exp(nodes[i] -> score / softmax_T);
    }

    // ソフトマックス法を使って確率を算出
    for (i = 0;i < node_num;i++) {
        nodes[i] -> chance = exp(nodes[i] -> score / softmax_T) / sum_exp;
    }

}


/*
評価値を伝搬させる
評価値 = sum(-(子ノードの評価点) * ルーレット選択の確率)
つまり、子ノードの期待値が親ノードの評価値になる。
評価点をマイナス倍しているのは、相手のスコア=-(自分のスコア)だから。
*/
void propagation(Game *game, Node *node) {
    // 伝搬の前に選択確率を更新
    chance_update(node -> children, node -> child_num);

    int i;
    double score = 0;
    for (i = 0; i < node -> child_num;i++) {
        score += node -> children[i] -> score * node -> children[i] -> chance;
    }
    node -> score = -score;

    if (node -> parent == NULL) {
        // rootノード
        return;
    }

    propagation(game, node -> parent);
}


/*
探索に使用したノードを開放する関数。
rootノードを渡すと、再帰的に開放していく。
*/
void all_free(Node *node) {
    if (node -> child_num == 0) {
        free(node);
        node = NULL;
        return;
    }
    int i;
    for (i = 0;i < node -> child_num;i++){
        all_free(node -> children[i]);
    }

    free(node);
    node = NULL;
    return;
}


/*
ソフトマックス探索を行う関数。
int pos[]の中に次に打つべき手を格納する。
*/
void bot_softmax(Game *game, int pos[]) {
    const int max_count = 7000;
    int count;

    // 各着手可能位置を調査
    char putpos[30][2];
    int length = make_putlist(game -> reverse, putpos);

    // 探索時間短縮のため、置ける場所が1箇所なら即return
    if (length == 1) {
        pos[0] = putpos[0][0];
        pos[1] = putpos[0][1];
        return;
    }

    int i;

    // rootは今現在の盤面。targetは探索する末端ノード
    Node *root, *target;

    root  = (Node *)malloc(sizeof(Node));

    node_init(root, NULL, game -> board, 0, game -> turn);

    for (i = 0;i < max_count;i++) {
        // 末端ノードを求める
        target = search(root);
        // 末端ノードを展開し、子ノードを広げる。
        expand(game, target);
        // 広げたノードから評価値を反映させていく。
        propagation(game, target);
    }

    // 最終的に評価値が最大になったrootの子ノードを選択
    double max_score = root -> children[0] -> score;
    pos[0] = putpos[0][0];
    pos[1] = putpos[0][1];
    for (i = 0;i < length;i++) {
        // printf("pos(%d, %d) -> score: %lf\n", putpos[i][0], putpos[i][1], root -> children[i] -> score);
        if (max_score < root -> children[i] -> score) {
            max_score = root -> children[i] -> score;
            pos[0] = putpos[i][0];
            pos[1] = putpos[i][1];
        }
    }

    // nodeを開放する
    all_free(root);
}


typedef struct {
    double score;
    int pos[2];
} minimax_t;

double min_calc(Game *game, Node *node, int depth_limit, int alpha);
double max_calc(Game *game, Node *node, int depth_limit, int beta);

double max_calc(Game *game, Node *node, int depth_limit, int beta) {

    // 深さ制限に達していたら返す。
    if (depth_limit <= 0) return evaluation(node -> board, game -> turn);

    char putpos[30][2];
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2] = {0};

    int length = make_board_to_putlist(node -> board, node -> player, reverse, putpos);
    int result = 0;

    int i, j, k;
    double score, max_score = MIN_SCORE;

    if (length == 0) {
        length = make_board_to_putlist(node -> board, opponent(node -> player), reverse, putpos);
        if (length == 0) {
            // 決着ノード
            result = count_board(node -> board, game -> turn);
            if (result == 1) {
                return MAX_SCORE;
            } else if (result == -1) {
                return MIN_SCORE;
            } else if (result == 0) {
                return 0;
            }
        } else {
            // パスノード
            // 全く同じ盤面を作り、擬似的につなげる。
            node -> children[0] = (Node *)malloc(sizeof(Node));
            node_init(node -> children[0], node, node -> board, 0, opponent(node -> player));
            node -> child_num = 1;

            return min_calc(game, node -> children[0], depth_limit - 1, max_score);
        }
    }

    char putted_board[BOARD_SIZE+2][BOARD_SIZE+2];
    int pos[2];

    // 通常ノード
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
        node_init(node -> children[i], node, putted_board, 0, opponent(node -> player));
        node -> child_num++;
        score = min_calc(game, node -> children[i], depth_limit - 1, max_score);

        if (score > max_score) {
            max_score = score;
        }

        if (beta < max_score) {
            // 探索を打ち切る
            break;
        }
    }

    return max_score;
}


double min_calc(Game *game, Node *node, int depth_limit, int alpha) {

    // 深さ制限に達していたら返す。
    if (depth_limit <= 0) return evaluation(node -> board, game -> turn);

    char putpos[30][2];
    char reverse[BOARD_SIZE+2][BOARD_SIZE+2] = {0};

    int length = make_board_to_putlist(node -> board, node -> player, reverse, putpos);
    int result = 0;

    int i, j, k;
    double score, min_score = MAX_SCORE;

    if (length == 0) {
        length = make_board_to_putlist(node -> board, opponent(node -> player), reverse, putpos);
        if (length == 0) {
            // 決着ノード
            result = count_board(node -> board, game -> turn);
            if (result == 1) {
                return MAX_SCORE;
            } else if (result == -1) {
                return MIN_SCORE;
            } else if (result == 0) {
                return 0;
            }
        } else {
            // パスノード
            // 全く同じ盤面を作り、擬似的につなげる。
            node -> children[0] = (Node *)malloc(sizeof(Node));
            node_init(node -> children[0], node, node -> board, 0, opponent(node -> player));
            node -> child_num = 1;

            return max_calc(game, node -> children[0], depth_limit - 1, min_score);
        }
    }

    char putted_board[BOARD_SIZE+2][BOARD_SIZE+2];
    int pos[2];

    // 通常ノード
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
        node_init(node -> children[i], node, putted_board, 0, opponent(node -> player));
        node -> child_num++;
        score = max_calc(game, node -> children[i], depth_limit - 1, min_score);

        if (score < min_score) {
            min_score = score;
        }

        if (min_score < alpha) {
            // 探索を切る
            break;
        }
    }

    return min_score;
}


void bot_alpha_beta(Game *game, int pos[]) {
    const int depth_limit = 6;

    char reverse[BOARD_SIZE+2][BOARD_SIZE+2];
    char putpos[30][2];
    int length = make_board_to_putlist(game -> board, game -> turn, reverse, putpos);
    int temp_pos[2];

    pos[0] = putpos[0][0];
    pos[1] = putpos[0][1];

    // 置く場所が1箇所しか無いならそのまま返す。
    if (length == 1){
        // printf("a\n");
        return;
    }

    Node *root = (Node *)malloc(sizeof(Node));
    node_init(root, NULL, game -> board, 0, EMPTY);

    char putted_board[BOARD_SIZE+2][BOARD_SIZE+2];
    int i, j, k;
    double score, max_score = MIN_SCORE;

    // 1手目(max)はここで行う
    for (i = 0;i < length;i++) {
        // boardをコピー
        for (j = 0;j < BOARD_SIZE+2;j++) {
            for (k = 0;k < BOARD_SIZE+2;k++) {
                putted_board[j][k] = game -> board[j][k];
            }
        }

        temp_pos[0] = putpos[i][0];
        temp_pos[1] = putpos[i][1];
        put_stone_test(putted_board, reverse, temp_pos, game -> turn);
        root -> children[i] = (Node *)malloc(sizeof(Node));
        node_init(root -> children[i], root, putted_board, 0, opponent(game -> turn));
        root -> child_num++;
        score = min_calc(game, root -> children[i], depth_limit - 1, max_score);

        if (score > max_score) {
            max_score = score;
            pos[0] = putpos[i][0];
            pos[1] = putpos[i][1];
        }        
    }


    all_free(root);
}
