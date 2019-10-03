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


/*
    人間がキーボードから入力を行う用のロジック
*/
void player(struct Game *game, int pos[]) {
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

void bot_random(struct Game *game, int pos[]) {
    char putpos[60][2];
    int index = 0;

    int x, y;

    for (y = 1; y < BOARD_SIZE+1;y++) {
        for (x = 1;x < BOARD_SIZE+1;x++) {
            if (game -> reverse[y][x] != 0) {
                putpos[index][0] = y;
                putpos[index][1] = x;

                index++;
            }
        }
    }

    index = rand() % index; // 置ける場所からランダムに一つ
    pos[0] = putpos[index][0];
    pos[1] = putpos[index][1];
}