#ifndef UTILITY_H
#define UTILITY_H

#include "reversi.h"

extern int c2i(char c);
extern int input(char command[]);
extern int make_putlist(char reverse[BOARD_SIZE+2][BOARD_SIZE+2], int putlist[][2]);
extern int len(int arr[]);
extern int len(double arr[]);
extern int min(int arr[]);
extern int min(double arr[]);

#endif