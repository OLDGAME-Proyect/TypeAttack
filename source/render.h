#ifndef RENDER_H
#define RENDER_H

#ifdef _WIN32
    #include <curses.h>   
#else
    #include <ncurses.h>  
#endif


void render_clear(void);

void render_present(void);

void render_input_bar(void);

void render_sentence(const char *text, int x, int y);

#endif
