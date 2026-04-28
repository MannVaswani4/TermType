#include "mystring.h"
#include "memory.h"
#include "words.h"
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
    /* First pass: pick words and compute total length */
    int *chosen = (int *)my_alloc(sizeof(int) * num_words);
    if (!chosen) return "";

    int total_len = 0;
    int i;
    for (i = 0; i < num_words; i++) {
        chosen[i] = rand() % common_words_count;
        total_len += my_strlen(common_words[chosen[i]]);
    }
    total_len += num_words - 1; /* spaces between words */
    total_len += 1;             /* null terminator */

    char *sentence = (char *)my_alloc(total_len);
    if (!sentence) return "";

    int pos = 0;
    for (i = 0; i < num_words; i++) {
        if (i > 0) {
            sentence[pos++] = ' ';
        }
        const char *w = common_words[chosen[i]];
        int wlen = my_strlen(w);
        int j;
        for (j = 0; j < wlen; j++) {
            sentence[pos++] = w[j];
        }
    }
    sentence[pos] = '\0';

    return sentence;
}