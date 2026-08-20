#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif
#include "menu.h"
#include "score.h"

int menu(void)
{
    int selected = 0;
    nodelay(stdscr, FALSE);

    while (1)
    {
        erase();

        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        int total_score = score_load();
        int global_level = (total_score <= 0) ? 1 : 1 + (total_score / 300);

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(max_y / 2 - 5, (max_x / 2) - 8, "  TYPE ATTACK!  ");
        attroff(COLOR_PAIR(1) | A_BOLD);

        if (selected == 0) {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(max_y / 2 - 1, (max_x / 2) - 5, "> | START | <");
            attroff(COLOR_PAIR(4) | A_BOLD);
        } else {
            mvprintw(max_y / 2 - 1, (max_x / 2) - 5, "  |START|  ");
        }

        if (selected == 1) {
            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(max_y / 2 + 1, (max_x / 2) - 5, "> | EXIT |  <");
            attroff(COLOR_PAIR(4) | A_BOLD);
        } else {
            mvprintw(max_y / 2 + 1, (max_x / 2) - 5, "  |EXIT|   ");
        }

        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(max_y / 2 + 4, (max_x / 2) - 17, "PLAYER LEVEL: %d  |  TOTAL SCORE: %d PTS", global_level, total_score);
        attroff(COLOR_PAIR(2) | A_BOLD);

        attron(COLOR_PAIR(8));
        mvprintw(max_y / 2 + 6, (max_x / 2) - 16, "| [UP/DOWN] NAVIGATE  | [ENTER] OK | [ESC] EXIT |");
        attroff(COLOR_PAIR(8));

        refresh();

        int ch = getch();
        if (ch == KEY_UP || ch == 'w' || ch == 'W') selected = 0;
        if (ch == KEY_DOWN || ch == 's' || ch == 'S') selected = 1;
        if (ch == '\n' || ch == '\r' || ch == 10 || ch == KEY_ENTER) return selected;
        if (ch == 27) return 1;
    }
}
