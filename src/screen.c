#include "screen.h"

/* write() is a raw POSIX syscall — no stdio needed */
#include <unistd.h>
#include "mystring.h"

void screen_clear(void) {
    /* ANSI escape: clear screen and move cursor to top-left */
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
}

void screen_print(const char *s) {
    write(STDOUT_FILENO, s, my_strlen(s));
}

void screen_println(const char *s) {
    screen_print(s);
    write(STDOUT_FILENO, "\n", 1);
}