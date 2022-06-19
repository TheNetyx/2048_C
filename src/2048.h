#ifndef _2048_H_
#define _2048_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

typedef int Grid;

#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_RIGHT 2
#define DIR_LEFT  3

#define GRID_BUF_SIZE (GRID_BUF_CHARS_PER_ROW * GRID_BUF_NUM_ROWS)
#define GRID_BUF_CHARS_PER_ROW 31
#define GRID_BUF_NUM_ROWS 17

#define clearscreen() printf("\033[2J\033[H")

#define GRID(y, x)  grid[((y) * 4 + (x))]

const int LOCATION_MODIFIER[] = {
    64,  71,  78,  85,
    188, 195, 202, 209,
    312, 319, 326, 333,
    436, 443, 450, 457
};

int  add_random(Grid *grid);
void draw_board(Grid *grid);
bool make_move(Grid *grid, const int dir);
void game_over(Grid *grid, bool won);
bool has_valid_moves(Grid *grid);

#endif /* _2048_H_ */
