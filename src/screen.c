#include "screen.h"
#include "mystring.h"
#include <unistd.h>

void screen_print(const char *s) {
    write(STDOUT_FILENO, s, my_strlen(s));
}

void screen_println(const char *s) {
    screen_print(s);
    screen_print("\n");
}

void screen_print_int(int n) {
    if (n == 0) {
        screen_print("0");
        return;
    }
    char buf[16];
    int i = 14;
    buf[15] = '\0';
    int neg = 0;
    if (n < 0) {
        neg = 1;
        n = -n;
    }
    while (n > 0) {
        buf[i--] = '0' + (n % 10);
        n /= 10;
    }
    if (neg) buf[i--] = '-';
    screen_print(&buf[i + 1]);
}

void screen_move(int row, int col) {
    screen_print("\x1b[");
    screen_print_int(row);
    screen_print(";");
    screen_print_int(col);
    screen_print("H");
}

void screen_print_char_colored(char c, int state, int underline) {
    if (underline) {
        screen_print("\x1b[4m");
    }
    if (state == 1) {
        screen_print("\x1b[32m");        /* green  – correct      */
    } else if (state == -1) {
        screen_print("\x1b[31m");        /* red    – wrong        */
    } else if (state == -2) {
        screen_print("\x1b[91m");        /* bright red – overflow */
    } else if (state == 2) {
        screen_print("\x1b[38;5;141m");  /* purple – current word */
    } else {
        screen_print("\x1b[90m");        /* dim    – untyped      */
    }
    char buf[2] = {c, '\0'};
    screen_print(buf);
    screen_print("\x1b[0m");
}

void screen_clear(void) {
    screen_print("\x1b[2J");
    screen_move(1, 1);
}
