#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif
#include <string.h>
#include <stdio.h>
#include "render.h"

void render_init(void)
{
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_BLUE, COLOR_BLACK);
    init_pair(8, COLOR_WHITE, COLOR_BLACK);
}

void render_clear(void)
{
    erase();
}

void render_present(void)
{
    refresh();
}

void render_sentence_colored(const char *target, const char *input, int x, int y, int rainbow_tick)
{
    if (!target) return;

    int target_len = (int)strlen(target);
    int input_len = input ? (int)strlen(input) : 0;

    static const int rainbow_pairs[] = { 2, 4, 6, 5, 7 };
    const int rainbow_count = 5;

    for (int i = 0; i < target_len; i++) {
        if (i < input_len) {
            if (input[i] == target[i]) {
                int pair = rainbow_pairs[(i + rainbow_tick) % rainbow_count];
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(y, x + i, target[i]);
                attroff(COLOR_PAIR(pair) | A_BOLD);
            } else {
                attron(COLOR_PAIR(3) | A_BOLD);
                mvaddch(y, x + i, target[i]);
                attroff(COLOR_PAIR(3) | A_BOLD);
            }
        } else {
            attron(COLOR_PAIR(8));
            mvaddch(y, x + i, target[i]);
            attroff(COLOR_PAIR(8));
        }
    }
}

void render_console_sidebar(int start_x, int max_y, const EventConsole *console)
{
    int limit_y = max_y - 2;

    attron(COLOR_PAIR(5));
    for (int y = 0; y < limit_y; y++) {
        mvaddch(y, start_x, ACS_VLINE);
    }
    attroff(COLOR_PAIR(5));

    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(1, start_x + 2, " logs ");
    attroff(COLOR_PAIR(1) | A_BOLD);

    if (console) {
        int draw_y = 3;
        for (int i = 0; i < console->count && draw_y < limit_y - 1; i++) {
            attron(COLOR_PAIR(console->events[i].color_pair) | A_BOLD);
            mvprintw(draw_y, start_x + 2, "%s", console->events[i].text);
            attroff(COLOR_PAIR(console->events[i].color_pair) | A_BOLD);
            draw_y += 1;
        }
    }
}

void render_input_bar(const char *input_buf, int level, int score, int max_y, int max_x)
{
    attron(COLOR_PAIR(5));
    for (int x = 0; x < max_x; x++) {
        mvaddch(max_y - 2, x, ACS_HLINE);
    }
    attroff(COLOR_PAIR(5));

    attron(COLOR_PAIR(8));
    mvprintw(max_y - 1, 1, "> %s", input_buf ? input_buf : "");
    attroff(COLOR_PAIR(8));

    int input_len = input_buf ? (int)strlen(input_buf) : 0;
    attron(A_BLINK | A_BOLD);
    mvaddch(max_y - 1, 3 + input_len, '_');
    attroff(A_BLINK | A_BOLD);

    char status_str[48];
    snprintf(status_str, sizeof(status_str), "[ lvl: %d | score: %d PTS ]", level, score);
    int status_x = max_x - (int)strlen(status_str) - 2;
    if (status_x > 3 + input_len + 2) {
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw(max_y - 1, status_x, "%s", status_str);
        attroff(COLOR_PAIR(4) | A_BOLD);
    }
}

void log_console_event(EventConsole *console, const char *text, int color_pair)
{
    if (!console || !text) return;

    if (console->count < MAX_LOG_EVENTS) {
        strncpy(console->events[console->count].text, text, sizeof(console->events[console->count].text) - 1);
        console->events[console->count].text[sizeof(console->events[console->count].text) - 1] = '\0';
        console->events[console->count].color_pair = color_pair;
        console->count++;
    } else {
        for (int i = 0; i < MAX_LOG_EVENTS - 1; i++) {
            console->events[i] = console->events[i + 1];
        }
        strncpy(console->events[MAX_LOG_EVENTS - 1].text, text, sizeof(console->events[MAX_LOG_EVENTS - 1].text) - 1);
        console->events[MAX_LOG_EVENTS - 1].text[sizeof(console->events[MAX_LOG_EVENTS - 1].text) - 1] = '\0';
        console->events[MAX_LOG_EVENTS - 1].color_pair = color_pair;
    }
}
