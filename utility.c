#include "const.h"
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

int make_putlist(char reverse[BOARD_SIZE+2][BOARD_SIZE+2], int putlist[][2]) {
    int length = 0;
    int x, y;
    for (y = 1;y < BOARD_SIZE+1;y++) {
        for (x = 1;x < BOARD_SIZE+1;x++) {
            if (reverse[y][x] != 0) {
                putlist[length][0] = y;
                putlist[length][1] = x;
                length++;
            }
        }
    }

    return length;
}

int make_reverse_test(char board[BOARD_SIZE+2][BOARD_SIZE+2],
                 int player,
                 char reverse[BOARD_SIZE+2][BOARD_SIZE+2],
                 int putlist[][2]) {
    /*
    playerとboardからひっくり返せる場所を見つけて返す。
    返り値は置けるかどうか。TRUEで置ける。
    */

   int y, x;
   int dy, dx;
   int i;
   int index = 0;

    for (y = 1;y <= BOARD_SIZE;y++) {
        for (x = 1;x <= BOARD_SIZE;x++) {
            reverse[y][x] = 0; // 初期化
            if (board[y][x] != EMPTY) continue;
            for (i = 0;i < 8;i++) {
                dy = y + DIRECTION[i][0];
                dx = x + DIRECTION[i][1];
                if (board[dy][dx] == opponent(player)) {
                    do {
                        dy += DIRECTION[i][0];
                        dx += DIRECTION[i][1];
                    } while (board[dy][dx] == opponent(player));
                    
                    if (board[dy][dx] == player) {
                        reverse[y][x] |= FLAG[i];
                        putlist[index][0] = y;
                        putlist[index][1] = x;
                        index++;
                    }
                }
            }
        }
    }

    return index;
}

int len_i(int arr[]) {
    return sizeof(arr) / sizeof(arr[0]);
}

int len_d(double arr[]) {
    return sizefo(arr) / sizeof(arr[0]);
}

int min_i(int arr[]) {
    int i = 0;
    int num = arr[0];
    for (i = 0;i < len(arr);i++) {
        if (num > arr[i]) {
            num = arr[i];
        }
    }

    return num;
}

int min_d(double arr[]) {
    int i = 0;
    int num = arr[0];
    for (i = 0;i < len(arr);i++) {
        if (num > arr[i]) {
            num = arr[i];
        }
    }

    return num;
}