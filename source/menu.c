#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif

int menu(void)
{
    int selected = 0; // 0=start, 1=exit

    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); 

    while (1)
    {
        clear();

        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        // ===================== TITLE =====================
        attron(COLOR_PAIR(1));
        mvprintw(max_y/2 - 4, (max_x/2)-6, " Type Attack! ");
        attroff(COLOR_PAIR(1)); 


        // ===================== OPTIONS ====================
        if (selected == 0)
            mvprintw(max_y/2, (max_x/2)-3, "_Start_");
        else
            mvprintw(max_y/2, (max_x/2)-3, " Start ");

        if (selected == 1)
            mvprintw(max_y/2 + 2, (max_x/2)-3, "_Exit_");
        else
            mvprintw(max_y/2 + 2, (max_x/2)-3, " Exit ");


        refresh();

        int ch = getch();
        if (ch == KEY_UP) selected = 0;
        if (ch == KEY_DOWN) selected = 1;
        if (ch == '\n') return selected;
    }
}
