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