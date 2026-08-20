#ifndef RENDER_H
#define RENDER_H

#define MAX_LOG_EVENTS 8

typedef struct {
    char text[48];
    int color_pair;
} LogEvent;

typedef struct {
    LogEvent events[MAX_LOG_EVENTS];
    int count;
} EventConsole;

void render_init(void);
void render_clear(void);
void render_present(void);

void render_sentence_colored(const char *target, const char *input, int x, int y, int rainbow_tick);
void render_console_sidebar(int start_x, int max_y, const EventConsole *console);
void render_input_bar(const char *input_buf, int level, int score, int max_y, int max_x);

void log_console_event(EventConsole *console, const char *text, int color_pair);

#endif
