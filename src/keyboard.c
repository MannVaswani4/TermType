#include "keyboard.h"
#include <stdio.h>

int keyboard_readline(char *buf, int len) {
    int i = 0;
    char c;

    while (i < len - 1) {
        c = getchar();

        if (c == EOF || c == '\n') break;

        buf[i++] = c;
    }

    buf[i] = '\0';
    return i;
}