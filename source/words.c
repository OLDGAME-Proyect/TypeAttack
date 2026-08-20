#include "words.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

typedef struct {
    char **items;
    int count;
    int capacity;
} WordBank;

static WordBank g_words = {0};
static WordBank g_congrats = {0};
static WordBank g_loser = {0};

static void bank_add(WordBank *bank, const char *str)
{
    if (bank->count >= bank->capacity) {
        int new_cap = (bank->capacity == 0) ? 32 : bank->capacity * 2;
        char **new_items = (char **)realloc(bank->items, new_cap * sizeof(char *));
        if (!new_items) return;
        bank->items = new_items;
        bank->capacity = new_cap;
    }

    size_t len = strlen(str);
    char *copy = (char *)malloc(len + 1);
    if (copy) {
        memcpy(copy, str, len + 1);
        bank->items[bank->count++] = copy;
    }
}

static void bank_free(WordBank *bank)
{
    if (bank->items) {
        for (int i = 0; i < bank->count; i++) {
            free(bank->items[i]);
        }
        free(bank->items);
        bank->items = NULL;
    }
    bank->count = 0;
    bank->capacity = 0;
}

static FILE *open_relative_file(const char *filename)
{
    char path[512];
    FILE *f = fopen(filename, "r");
    if (f) return f;

    snprintf(path, sizeof(path), "build/Debug/%s", filename);
    f = fopen(path, "r");
    if (f) return f;

    snprintf(path, sizeof(path), "../%s", filename);
    f = fopen(path, "r");
    if (f) return f;

    snprintf(path, sizeof(path), "../../%s", filename);
    f = fopen(path, "r");
    if (f) return f;

#ifdef _WIN32
    char exe_path[MAX_PATH];
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) > 0) {
        char *last_slash = strrchr(exe_path, '\\');
        if (last_slash) {
            *(last_slash + 1) = '\0';
            strcat_s(exe_path, MAX_PATH, filename);
            f = fopen(exe_path, "r");
            if (f) return f;
        }
    }
#endif

    return NULL;
}

static void load_bank_from_file(WordBank *bank, const char *filename, const char *fallback[], int fallback_count)
{
    bank_free(bank);

    FILE *f = open_relative_file(filename);
    if (f) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), f)) {
            size_t len = strlen(buffer);
            while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r' || buffer[len - 1] == ' ')) {
                buffer[--len] = '\0';
            }
            if (len > 0) {
                bank_add(bank, buffer);
            }
        }
        fclose(f);
    }

    if (bank->count == 0) {
        for (int i = 0; i < fallback_count; i++) {
            bank_add(bank, fallback[i]);
        }
    }
}

static const char *g_fallback_words[] = {
    "were ready?", "Fast typing may saves lives", " Go !",
    "Optimization never was a option", "radical and simplicity", "Im typing, you can see?"
};

static const char *g_fallback_congrats[] = {
    "Fiumba", "Precission!", "overspeed!", "No ones can do like me"
};

static const char *g_fallback_loser[] = {
    "my grandpa is using the game", "Pelao", "Total Miss", "Need a breake?"
};

int words_init(void)
{
    load_bank_from_file(&g_words, "words", g_fallback_words, sizeof(g_fallback_words) / sizeof(g_fallback_words[0]));
    load_bank_from_file(&g_congrats, "congrats", g_fallback_congrats, sizeof(g_fallback_congrats) / sizeof(g_fallback_congrats[0]));
    load_bank_from_file(&g_loser, "loser", g_fallback_loser, sizeof(g_fallback_loser) / sizeof(g_fallback_loser[0]));
    return g_words.count;
}

const char *words_get_random(void)
{
    if (g_words.count == 0) return "Type Type Type!";
    return g_words.items[rand() % g_words.count];
}

const char *congrats_get_random(void)
{
    if (g_congrats.count == 0) return "Flawless!";
    return g_congrats.items[rand() % g_congrats.count];
}

const char *loser_get_random(void)
{
    if (g_loser.count == 0) return "turtle!";
    return g_loser.items[rand() % g_loser.count];
}

void words_free(void)
{
    bank_free(&g_words);
    bank_free(&g_congrats);
    bank_free(&g_loser);
}
