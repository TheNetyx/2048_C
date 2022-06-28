#ifndef _2048_H_
#define _2048_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>

typedef int Grid;

#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_RIGHT 2
#define DIR_LEFT  3

#define GRID_BUF_SIZE (GRID_BUF_CHARS_PER_ROW * GRID_BUF_NUM_ROWS)
#define GRID_BUF_CHARS_PER_ROW 39
#define GRID_BUF_NUM_ROWS 17

#define clearscreen() printf("\033[2J\033[H")

#define GRID(y, x)  grid[((y) * 4 + (x))]

const int LOCATION_MODIFIER[] = {
    80,  89,  98,  107,
    236, 245, 254, 263,
    392, 401, 410, 419,
    548, 557, 566, 575
};

int  add_random(Grid *grid);
void draw_board(Grid *grid);
bool make_move(Grid *grid, const int dir);
void game_over(Grid *grid, bool won);
bool has_valid_moves(Grid *grid);

#endif /* _2048_H_ */
