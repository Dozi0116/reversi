#include <stdio.h>
#include "reversi.h"

#define buff_erase() while(getchar() != '\n')

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
        buff_erase();

        if (
            ('1' <= command[0] && command[0] <= '8') &&
            (
                ('a' <= command[1] && command[1] <= 'h') ||
                ('A' <= command[1] && command[1] <= 'H')
            )
        ) {
            return;
        } else {
            printf("input error. Please try again.\n");
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
        while (TRUE) {
            input(command);

            pos[0] = c2i(command[0]);
            pos[1] = c2i(command[1]) + 1; // 文字「A」がインデックス「1」に対応。

            if (game.reverse[pos[0]][pos[1]] != 0) {
                break;
            } else {
                printf("position error. Please try again.\n");
            }
        }

        printf("press -> (%d(%d), %c(%d))\n", pos[0], pos[0], pos[1] + 'A' - 1, pos[1]);
        put_stone(&game, pos, game.turn);

        if (next_turn(&game) == TRUE) {
            break;
        }
    }

    printf("game set!");
    show_board(&game);

    
    return 0;
}