#include "reversi.h"
#include "book_evaluation.h"
#include <stdio.h>

/*
compile
> gcc board_test.c book_evaluation.c reversi.c
*/

void show(char board[BOARD_SIZE+2][BOARD_SIZE+2]) {
    int i, j;
    int w_count = 0;
    int b_count = 0;

    printf("    A B C D E F G H\n");
    for (i = 1;i < BOARD_SIZE+1;i++) {
        printf("%d   ", i);
        for (j = 1;j < BOARD_SIZE+1;j++) {
            switch ((board)[i][j]) {
                case BLACK:
                    b_count++;
                    printf("o ");
                    break;
                case WHITE:
                    w_count++;
                    printf("x ");
                    break;
                case EMPTY:
                    printf(". ");
                    break;
                default:
                    printf("error");
            }
        }
        printf("\n");
    }
    
    printf("\nstatus\n");
    printf("o -> %d, x -> %d\n", b_count, w_count);
}

int main(void) {
    char board[BOARD_SIZE+2][BOARD_SIZE+2] = {
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, BLACK, BLACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, BLACK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, WHITE, BLACK, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, BLACK, WHITE, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}
    };

    printf("\nvvv input board vvv\n");
    show(board);
    printf("^^^ input board ^^^\n\n\n");
    double score = evaluation(board, BLACK);
    printf("BLACK score -> %lf\n", score);
}