#ifndef GAME_H
#define GAME_H

typedef struct {
    const char *text;
    float x;
    float y;
} Sentence;

void game_loop(const char *difficulty);

#endif
