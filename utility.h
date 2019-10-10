#ifndef UTILITY_H
#define UTILITY_H

#include "reversi.h"

extern int c2i(char c);
extern int input(char command[]);
extern int make_putlist(char reverse[BOARD_SIZE+2][BOARD_SIZE+2], int putlist[][2]);
extern int make_reverse_test(char board[BOARD_SIZE+2][BOARD_SIZE+2], int player, char reverse[BOARD_SIZE+2][BOARD_SIZE+2], int putlist[][2]);
extern int len_i(int arr[]);
extern int len_d(double arr[]);
extern int min_i(int arr[]);
extern int min_d(double arr[]);

#endif