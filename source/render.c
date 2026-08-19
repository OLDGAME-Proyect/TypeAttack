#ifdef _WIN32
    #include <curses.h>   
#else
    #include <ncurses.h>  
#endif
#include "render.h"

void render_clear(void)
{
    clear();
}

void render_present(void)
{
    refresh();
}

void render_input_bar(void)
{
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    mvprintw(max_y - 2, 0, "_____________________________________________________________________");
    mvprintw(max_y - 1, 0, "uw: ");
}

void render_sentence(const char *text, int x, int y)
{
    mvprintw(y, x, "%s", text);
}
