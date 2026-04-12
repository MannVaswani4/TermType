#include "mystring.h"

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

void my_strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int my_itoa(int n, char *buf) {
    int i = 0;
    if (n == 0) { buf[i++] = '0'; buf[i] = '\0'; return i; }
    if (n < 0)  { buf[i++] = '-'; n = -n; }
    /* Find digits in reverse order */
    char tmp[12];
    int  tlen = 0;
    while (n > 0) { tmp[tlen++] = '0' + (n % 10); n /= 10; }
    /* Reverse into buf */
    for (int j = tlen - 1; j >= 0; j--) buf[i++] = tmp[j];
    buf[i] = '\0';
    return i;
}

const char *my_strchr(const char *s, char c) {
    while (*s != '\0') {
        if (*s == c) return s;
        s++;
    }
    return 0;
}

int my_tokenize(char *src, char delim, char **tokens, int max_tokens) {
    int count = 0;
    tokens[count++] = src;
    while (*src != '\0' && count < max_tokens) {
        if (*src == delim) {
            *src = '\0';
            tokens[count++] = src + 1;
        }
        src++;
    }
    return count;
}