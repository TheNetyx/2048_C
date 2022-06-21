#include "2048.h"

int score;
int highscore = 0;
bool hs_enabled = true;

int main(void)
{
    Grid grid[16] = {}; // 4 x 4 grid, access cells using GRID(y, x)
    int key_pressed;
    FILE *hs_file;      // hs = highscore
    char *hs_file_name, *home_env;

    score = 0;

    home_env = getenv("HOME");
    if ((!home_env) || !(hs_file_name = malloc(strlen(home_env) + 17))) {
        hs_enabled = false;
    } else {
        sprintf(hs_file_name, "%s/.2048_highscores", home_env);
        hs_file = fopen(hs_file_name, "ab+");
        if(!hs_file) {
            /* free hs_file_name now because it is not freed at the end if  */
            /* hs_enabled is false because the file cannot be opened        */
            free(hs_file_name);
            hs_enabled = false;
        } else {
            /* don't care if this fails, since highscore defaults to 0 */
            fread(&highscore, sizeof(int), 1, hs_file);
            fseek(hs_file, 0, SEEK_SET);
        }
    }

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
                        goto cleanup;
                    }
                }
                if(add_random(grid)) {
                    game_over(grid, false);
                    goto cleanup;
                    //break;
                }
                draw_board(grid);
            }
        }
    }

    cleanup:
    if(hs_enabled) {
        if(score > highscore) {
            fwrite(&score, sizeof(int), 1, hs_file);
        }
        fclose(hs_file);
        free(hs_file_name);
    }
    endwin();
    return 0;
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
        /* this should never happen, as gameover is checked after the 16th */
        /* block is spawned, therefore making it impossible to attempt to  */
        /* spawn a block when the board is already full                    */
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
        /* if there are 15 cells filled, only 1 is open for spawning.       */
        /* game over if there are no valid moves after spawning new number. */
        return !has_valid_moves(grid);
    }

    return 0;
}

bool has_valid_moves(Grid *grid)
{
    for(int j = 0;j < 4;j++) {
        for(int i = 1;i < 4;i++) {
               /* up */                        /* left */
            if(GRID(i, j) == GRID(i - 1, j) || GRID(j, i) == GRID(j, i - 1)) {
                return true;
            }
        }
    }
    for(int j = 0;j < 4;j++) {
        for(int i = 2;i >= 0;i--) {
               /* down */                      /* right */
            if(GRID(i, j) == GRID(i + 1, j) || GRID(j, i) == GRID(j, i + 1)) {
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

    for(int i = 0;i < 16;i++) {
        if(grid[i]) {
            sprintf(buffer + LOCATION_MODIFIER[i], "%4d", grid[i]);
            /* replace null byte from sprintf with space so the grid can  */
            /* be printed properly                                        */
            *(buffer + LOCATION_MODIFIER[i] + 4) = ' ';
        }
    }

    clearscreen();
    printf(hs_enabled ?
           "+---------+---------+\r\n"
           "|  SCORE  |HIGHSCORE|\r\n"
           "+---------+---------+\r\n"
           "|  %6d |  %6d |\r\n"
           "+---------+---------+\r\n"
           :
           "+---------+--------+\r\n"
           "|  SCORE  | %6d |\r\n"
           "+---------+--------+\r\n",
           score, highscore
          );
    fwrite(buffer, 1, GRID_BUF_SIZE, stdout);
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

            /* start moving from the uppermost column because so all cells */
            /* move out of the way of other cells                          */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[i][j] && !newgrid[i - 1][j]) {
                        newgrid[i - 1][j] = newgrid[i][j];
                        newgrid[i][j] = 0;
                        /* if any grid is moved, move its column again.    */
                        /* i is set to 0 so that when it is incremented    */
                        /* by the loop it goes back to its initial value   */
                        i = 0;

                        /* if the cells were moved, this is a valid move   */
                        was_valid = true;
                    }
                }
            }


            /* merge */

            /* start merging from the uppermost column for the same reason */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[i][j] == newgrid[i - 1][j] && newgrid[i][j]) {
                        newgrid[i - 1][j] *= 2;
                        newgrid[i][j] = 0;
                        score += newgrid[i - 1][j];

                        /* if there was merges, this is a valid move */
                        was_valid = true;
                    }
                }
            }


            /* move everything again after merge, as merging creates holes */

            /* probably can merge this loop into the one above later       */
            for(int j = 0;j < 4;j++) {
                for(int i = 1;i < 4;i++) {
                    if(newgrid[i][j] && !newgrid[i - 1][j]) {
                        newgrid[i - 1][j] = newgrid[i][j];
                        newgrid[i][j] = 0;
                        i = 0;

                        /* was_valid not set because it would've been set  */
                        /* in the previous loop anyway                     */
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

    /* return value is negated because we return 0 for success */
    return !was_valid;
}

void game_over(Grid *grid, bool won)
{
    draw_board(grid);
    printf(won ? "You win!\r\n" : "GAME OVER\r\n");
    printf(score > highscore ? "New highscore: %d\r\n" : "Your score: %d\r\n", score);
    printf("Press q to exit...\r\n");

    for(;;) {
        if(getch() == 'q') {
            return;
        }
    }
}
