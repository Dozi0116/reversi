#ifndef LOGIC_H
#define LOGIC_H

#include "reversi.h"

/*
すべて共通の入力を受け取る
const struct Game *game ... 現在のゲームデータ。 試行錯誤したい場合はコピーを作ること。
int pos[] ... 置く予定の場所を格納する。 pos[0]は行(1～8)、pos[1]は列(1(A)～8(H))を入れる。
*/

// 対人戦のときに使う。人間が標準入力から受け取る。
extern void player(struct Game *game, int pos[]);

#endif