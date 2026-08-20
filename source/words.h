#ifndef WORDS_H
#define WORDS_H

int words_init(void);
const char *words_get_random(void);
const char *congrats_get_random(void);
const char *loser_get_random(void);
void words_free(void);

#endif
