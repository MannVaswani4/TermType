#include "mystring.h"
#include "memory.h"
#include <stdlib.h>

int my_strlen(const char *s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

int my_strcmp(const char *a, const char *b) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return a[i] - b[i];
        i++;
    }

    return a[i] - b[i];
}

char *generate_sentence(int num_words) {
    const char *words[] = {
        "the", "quick", "brown", "fox", "jumps",
        "over", "lazy", "dog", "hello", "world",
        "type", "fast", "code", "test", "run",
        "program", "keyboard", "screen", "memory", "data",
        "input", "output", "error", "debug", "compile"
    };
    int word_count = sizeof(words) / sizeof(words[0]);

    /* calculate total length needed */
    int total_len = 0;
    int chosen[20];
    for (int i = 0; i < num_words && i < 20; i++) {
        chosen[i] = rand() % word_count;
        total_len += my_strlen(words[chosen[i]]);
    }
    total_len += num_words - 1; /* spaces between words */
    total_len += 1;             /* null terminator */

    char *sentence = (char *)my_alloc(total_len);
    if (!sentence) return "";

    int pos = 0;
    for (int i = 0; i < num_words && i < 20; i++) {
        if (i > 0) {
            sentence[pos++] = ' ';
        }
        const char *w = words[chosen[i]];
        int wlen = my_strlen(w);
        for (int j = 0; j < wlen; j++) {
            sentence[pos++] = w[j];
        }
    }
    sentence[pos] = '\0';

    return sentence;
}