#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #undef MOUSE_MOVED
    #include <curses.h>
#else
    #include <ncurses.h>
    #include <unistd.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "render.h"
#include "words.h"
#include "score.h"

static float calculate_fall_speed(const char *difficulty, const char *text, int min_line, int level)
{
    int fall_time_ms = 18000;
    int letters = text ? (int)strlen(text) : 10;
    int words = 1;
    if (text) {
        for (int i = 0; text[i]; i++) {
            if (text[i] == ' ') words++;
        }
    }

    if (strcmp(difficulty, "00") == 0) {
        fall_time_ms = 18000;
    } else if (strcmp(difficulty, "01") == 0) {
        fall_time_ms = (rand() % 10 + 12) * 1000;
    } else if (strcmp(difficulty, "02") == 0) {
        fall_time_ms = 22000 - (letters * 300) - (words * 800);
        if (fall_time_ms < 6000) fall_time_ms = 6000;
    } else if (strcmp(difficulty, "03") == 0) {
        fall_time_ms = 14000 - (letters * 200) - (words * 500);
        if (fall_time_ms < 4000) fall_time_ms = 4000;
    }

    float total_frames = (float)fall_time_ms / 16.0f;
    if (total_frames < 1.0f) total_frames = 1.0f;
    
    float base_speed = (float)(min_line - 2) / total_frames;
    float level_multiplier = 1.0f + ((float)(level - 1) * 0.18f);
    return base_speed * level_multiplier;
}

void game_loop(const char *difficulty)
{
    nodelay(stdscr, TRUE);
    int running = 1;

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int sidebar_width = 28;
    int sidebar_x = (max_x > 65) ? (max_x - sidebar_width) : max_x;
    int play_width = (sidebar_x < max_x) ? (sidebar_x - 2) : (max_x - 2);
    int min_line = max_y - 3;

    EventConsole console = {0};
    int current_score = score_load();
    int correct_count = 0;
    int level = 1;

    Sentence s;
    s.text = words_get_random();
    s.y = 2.0f;
    int text_len = (int)strlen(s.text);
    int max_start_x = play_width - text_len;
    s.x = (max_start_x > 2) ? (float)(2 + (rand() % (max_start_x - 2))) : 2.0f;

    float fall_speed = calculate_fall_speed(difficulty, s.text, min_line, level);

    char input_buf[256] = {0};
    int input_len = 0;
    int tick = 0;

    while (running)
    {
        getmaxyx(stdscr, max_y, max_x);
        sidebar_x = (max_x > 65) ? (max_x - sidebar_width) : max_x;
        play_width = (sidebar_x < max_x) ? (sidebar_x - 2) : (max_x - 2);
        min_line = max_y - 3;

        int ch;
        while ((ch = getch()) != ERR)
        {
            if (ch == 27) {
                running = 0;
                break;
            }

            if (ch == KEY_BACKSPACE || ch == 8 || ch == 127 || ch == '\b') {
                if (input_len > 0) {
                    input_buf[--input_len] = '\0';
                }
            } else if (ch >= 32 && ch <= 126) {
                if (input_len < 254 && input_len < (int)strlen(s.text)) {
                    input_buf[input_len++] = (char)ch;
                    input_buf[input_len] = '\0';
                }
            }
        }

        if (!running) break;

        int target_len = (int)strlen(s.text);
        if (input_len == target_len && strcmp(input_buf, s.text) == 0) {
            correct_count++;
            int old_level = level;
            level = 1 + (correct_count / 3);

            current_score = score_add(100);
            log_console_event(&console, "+100 points", 2);
            log_console_event(&console, congrats_get_random(), 4);

            if (level > old_level) {
                log_console_event(&console, "## Level up! ##", 5);
            }

            s.text = words_get_random();
            target_len = (int)strlen(s.text);
            s.y = 2.0f;
            max_start_x = play_width - target_len;
            s.x = (max_start_x > 2) ? (float)(2 + (rand() % (max_start_x - 2))) : 2.0f;

            input_len = 0;
            input_buf[0] = '\0';
            fall_speed = calculate_fall_speed(difficulty, s.text, min_line, level);
        }

        s.y += fall_speed;

        if (s.y >= (float)min_line) {
            current_score = score_add(-100);
            log_console_event(&console, "-100 points", 3);
            log_console_event(&console, loser_get_random(), 3);

            s.text = words_get_random();
            target_len = (int)strlen(s.text);
            s.y = 2.0f;
            max_start_x = play_width - target_len;
            s.x = (max_start_x > 2) ? (float)(2 + (rand() % (max_start_x - 2))) : 2.0f;

            input_len = 0;
            input_buf[0] = '\0';
            fall_speed = calculate_fall_speed(difficulty, s.text, min_line, level);
        }

        render_clear();

        if (sidebar_x < max_x) {
            render_console_sidebar(sidebar_x, max_y, &console);
        }

        render_sentence_colored(s.text, input_buf, (int)s.x, (int)s.y, tick / 4);
        render_input_bar(input_buf, level, current_score, max_y, max_x);
        render_present();

#ifdef _WIN32
        Sleep(16);
#else
        usleep(16000);
#endif
        tick++;
    }
}
