#include "keyboard.h"
#include <unistd.h>

int keyboard_readline(char *buf, int len) {
    int i = 0;
    char c;

    while (i < len - 1) {
        int n = read(STDIN_FILENO, &c, 1);  
        if (n <= 0) break;                  
        if (c == '\n') break;               
        buf[i++] = c;
    }

    buf[i] = '\0';  
    return i;
}