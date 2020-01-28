#ifndef CONST_H
#define CONST_H

#define BOARD_SIZE 8
#define PLAYERS 2

#define BLACK 1
#define WHITE 0
#define EMPTY -1

#define TRUE 1
#define FALSE 0

#define MIN_SCORE -99999
#define MAX_SCORE 99999

#define UP_LEFT 0x80
#define UP_CENTER 0x40
#define UP_RIGHT 0x20
#define MID_LEFT 0x10
#define MID_RIGHT 0x08
#define DOWN_LEFT 0x04
#define DOWN_CENTER 0x02
#define DOWN_RIGHT 0x01

#define DIRECTION_SIZE 8
extern const int DIRECTION[DIRECTION_SIZE][2];

extern const int FLAG[DIRECTION_SIZE];

#endif