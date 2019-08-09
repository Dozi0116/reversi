#include <stdio.h>
#include "reversi.h"

int c2i(char c) {
    if ('A' <= c && c <= 'Z') {
        return c - 'A';
    } else if ('a' <= c && c <= 'z') {
        return c - 'a';
    } else if ('0' <= c && c <= '9') {
        return c - '0';
    } else {
        // error
        return -1;
    }
}

void input(char command[]) {
    while (TRUE) {
        scanf("%2[^\n]%*[^\n]", command);

        if (
            ('1' <= command[0] && command[0] <= '8') &&
            (
                ('a' <= command[1] && command[1] <= 'h') ||
                ('A' <= command[1] && command[1] <= 'H')
            )
        ) {
            return;
        } else {
            printf("error\n"); // TODO 不正な入力を入れると、以降scanfが正しく動作せず、無限ループに陥る。
        }
    }
}

int main(void) {
    Game game;
    game_init(&game);
    char command[3];
    int pos[2];

    printf("%d\n", game.turn);

    while (TRUE) {
        show_board(&game);
        printf("put pos (ex. 1A)-> ");
        input(command);

        pos[0] = c2i(command[0]);
        pos[1] = c2i(command[1]) + 1; // 文字「A」がインデックス「1」に対応。

        printf("press -> (%d(%d), %c(%d))\n", pos[0], pos[0], pos[1] + 'A' - 1, pos[1]);
        put_stone(&game, pos, game.turn);
        show_board(&game);
        break;
    }
    
    return 0;
}