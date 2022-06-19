#include "2048.h"

int score;

int main(void)
{
    Grid grid[16] = {}; // 4 x 4 grid, access cells using GRID(y, x)
    int key_pressed;
    score = 0;

    srand(time(NULL));
    /* turn off echoing of inputs */
    initscr();
    cbreak();
    noecho();

    /* for some reason it only starts printing after a call to getch */
    timeout(0);
    getch();

    /* set it to wait until input is received */
    timeout(-1);


    add_random(grid);
    draw_board(grid);


    for(;;) {
        key_pressed = getch();

        /* Up, Down, Right, Left arrows are 65, 66, 67, 68 respectively */
        if(key_pressed >= 65 && key_pressed <= 68) {
            if(!make_move(grid, key_pressed - 65)) {
                /* check if there is a cell with 2048 (win) */
                for(int i = 0;i < 16;i++) {
                    if(grid[i] == 2048) {
                        add_random(grid);
                        game_over(grid, true);
                    }
                }
                if(add_random(grid)) {
                    game_over(grid, false);
                }
                draw_board(grid);
            }
        }
    }

}

int add_random(Grid *grid)
{
    int index;
    bool is_4;
    int num_filled_cells = 0;

    /* check if board is full (no cells are 0) */
    for(int i = 0;i < 16;i++) {
        if(grid[i])
            num_filled_cells++;
    }
    if(num_filled_cells >= 16) {
        system("touch fullcell");
        abort();
    }

    /* 4 has a 1 in 10 chance of spawning */
    is_4 = !(rand() % 10);

    for(;;) {
        index = rand() % 16;
        if(!grid[index])
            break;
    }

    grid[index] = is_4 ? 4 : 2;

    if(num_filled_cells == 15) {
        /* if there are 15 cells filled, only 1 is open for spawning. */
        /* game over if there are no valid moves after spawning new number. */
        return !has_valid_moves(grid);
    }

    return 0;
}

bool has_valid_moves(Grid *grid)
{
    /* up */
    for(int j = 0;j < 4;j++) {
        for(int i = 1;i < 4;i++) {
            if(GRID(i, j) == GRID(i - 1, j)) {
                return true;
            }
        }
    }
    /* down */
    for(int j = 0;j < 4;j++) {
        for(int i = 2;i >= 0;i--) {
            if(GRID(i, j) == GRID(i + 1, j)) {
                return true;
            }
        }
    }
    /* left */
    for(int j = 0;j < 4;j++) {
        for(int i = 1;i < 4;i++) {
            if(GRID(j, i) == GRID(j, i - 1)) {
                return true;
            }
        }
    }
    /* right */
    for(int j = 0;j < 4;j++) {
        for(int i = 2;i >= 0;i--) {
            if(GRID(j, i) == GRID(j, i + 1)) {
                return true;
            }
        }
    }
    return false;
}

void draw_board(Grid *grid)
{
    char buffer[] = "+------+------+------+------+\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "+------+------+------+------+\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "+------+------+------+------+\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "+------+------+------+------+\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "|      |      |      |      |\r\n"
                    "+------+------+------+------+\r\n";
    char tempbuf[5];

    for(int i = 0;i < 16;i++) {
        if(grid[i]) {
            sprintf(tempbuf, "%4d", grid[i]);
            memcpy(buffer + LOCATION_MODIFIER[i], tempbuf, 4);
        }
    }

    clearscreen();
    fwrite(buffer, 1, GRID_BUF_SIZE, stdout);
    printf("SCORE: %d\r\n", score);
};

bool make_move(Grid *grid, const int dir)
{
    Grid newgrid[4][4] = {};
    bool merged[4][4] = {};
    bool was_valid = false;

    memcpy(newgrid, grid, sizeof(newgrid));

    switch(dir) {
        case DIR_UP:
            /* move everything in the direction specified */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[i][j] && !newgrid[i - 1][j]) {
                        newgrid[i - 1][j] = newgrid[i][j];
                        newgrid[i][j] = 0;
                        /* if any grid is moved, move this column again. */
                        /* these variables are set so that when they are */
                        /* incremented by the loop they start over again. */
                        i = 0;
                        was_valid = true;
                    }
                }
            }
            /* merge */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[i][j] == newgrid[i - 1][j] && newgrid[i][j]) {
                        newgrid[i - 1][j] *= 2;
                        newgrid[i][j] = 0;
                        score += newgrid[i - 1][j];
                        was_valid = true;
                    }
                }
            }
            /* move everything again */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[i][j] && !newgrid[i - 1][j]) {
                        newgrid[i - 1][j] = newgrid[i][j];
                        newgrid[i][j] = 0;
                        i = 0;
                    }
                }
            }
            break;

        case DIR_DOWN:
            /* move everything in the direction specified */
            for(int j = 0;j < 4;j++) {
                for(int i = 2;i >= 0;i--) {
                    if(newgrid[i][j] && !newgrid[i + 1][j]) {
                        newgrid[i + 1][j] = newgrid[i][j];
                        newgrid[i][j] = 0;
                        i = 2;
                        was_valid = true;
                    }
                }
            }
            /* merge */
            for(int j = 0;j < 4;j++) {
                for(int i = 2;i >= 0;i--) {
                    if(newgrid[i][j] == newgrid[i + 1][j] && newgrid[i][j]) {
                        newgrid[i + 1][j] *= 2;
                        newgrid[i][j] = 0;
                        score += newgrid[i + 1][j];
                        was_valid = true;
                    }
                }
            }
            /* move everything again */
            for(int j = 0;j < 4;j++) {
                for(int i = 2;i >= 0;i--) {
                    if(newgrid[i][j] && !newgrid[i + 1][j]) {
                        newgrid[i + 1][j] = newgrid[i][j];
                        newgrid[i][j] = 0;
                        i = 2;
                    }
                }
            }
            break;

        case DIR_LEFT:
            /* move everything in the direction specified */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[j][i] && !newgrid[j][i - 1]) {
                        newgrid[j][i - 1] = newgrid[j][i];
                        newgrid[j][i] = 0;
                        i = 0;
                        was_valid = true;
                    }
                }
            }
            /* merge */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[j][i] == newgrid[j][i - 1] && newgrid[j][i]) {
                        newgrid[j][i - 1] *= 2;
                        newgrid[j][i] = 0;
                        score += newgrid[j][i - 1];
                        was_valid = true;
                    }
                }
            }
            /* move everything again */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[j][i] && !newgrid[j][i - 1]) {
                        newgrid[j][i - 1] = newgrid[j][i];
                        newgrid[j][i] = 0;
                        i = 0;
                    }
                }
            }
            break;

        case DIR_RIGHT:
            /* move everything in the direction specified */
            for(int j = 0;j < 4;j++) {
                for(int i = 2;i >= 0;i--) {
                    if(newgrid[j][i] && !newgrid[j][i + 1]) {
                        newgrid[j][i + 1] = newgrid[j][i];
                        newgrid[j][i] = 0;
                        i = 3;
                        was_valid = true;
                    }
                }
            }
            /* merge */
            for(int j = 0;j < 4;j++) {
                for(int i = 2;i >= 0;i--) {
                    if(newgrid[j][i] == newgrid[j][i + 1] && newgrid[j][i]) {
                        newgrid[j][i + 1] *= 2;
                        newgrid[j][i] = 0;
                        score += newgrid[j][i + 1];
                        was_valid = true;
                    }
                }
            }
            /* move everything again */
            for(int j = 0;j < 4;j++) {
                for(int i = 2;i >= 0;i--) {
                    if(newgrid[j][i] && !newgrid[j][i + 1]) {
                        newgrid[j][i + 1] = newgrid[j][i];
                        newgrid[j][i] = 0;
                        i = 3;
                    }
                }
            }
            break;
        default:
            system("touch baddir");
            abort();
    }
    memcpy(grid, newgrid, sizeof(newgrid));

    return !was_valid;
}

void game_over(Grid *grid, bool won)
{
    draw_board(grid);
    printf(won ? "You win!\r\n" : "GAME OVER\r\n");
    printf("Final score: %d\r\n", score);
    printf("Press q to exit...\r\n");
    for(;;) {
        if(getch() == 'q') {
            endwin();
            exit(0);
        }
    }
}
