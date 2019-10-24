#ifndef LOGIC_H
#define LOGIC_H

#include "reversi.h"

/*
すべて共通の入力を受け取る
const struct Game *game ... 現在のゲームデータ。 試行錯誤したい場合はコピーを作ること。
int pos[] ... 置く予定の場所を格納する。 pos[0]は行(1～8)、pos[1]は列(1(A)～8(H))を入れる。
*/

typedef struct Node {
    /* ソフトマックス法で使う木 */
    struct Node *parent; // 親ノード
    struct Node *children[30]; // 子ノードたち
    char board[BOARD_SIZE+2][BOARD_SIZE+2]; // その時の盤面、できれば保持したくないが…
    int player; // このときに石を置く側
    int child_num; // childrenの要素数
    double score; // このノードの評価点
    double chance; // このノードの選出確率
} Node;

// 対人戦のときに使う。人間が標準入力から入力する。
extern void player(struct Game *game, int pos[]);

// 着手可能手のなかからランダムに1手選択。
extern void bot_random(struct Game *game, int pos[]);

// ソフトマックス関数に基づいて最善手を選択。
extern void bot_softmax(struct Game *game, int pos[]);

#endif