#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

int main(void)
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    render_init();
    srand((unsigned int)time(NULL));
    words_init();

    char difficulty[8] = "00";
    FILE *cfg = fopen("config", "r");
    if (cfg) {
        fscanf(cfg, "type=%7s", difficulty);
        fclose(cfg);
    } else {
        cfg = fopen("config", "w");
        if (cfg) {
            fprintf(cfg, "type=00");
            fclose(cfg);
        }
    }

    while (1) {
        int choice = menu();
        if (choice != 0) {
            break;
        }
        game_loop(difficulty);
    }

    words_free();
    endwin();
    return 0;
}
