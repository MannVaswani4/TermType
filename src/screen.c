#include "screen.h"
#include "mystring.h"
#include <stdio.h>

void screen_print(const char *s) {
    printf("%s", s);
}

void screen_println(const char *s) {
    printf("%s\n", s);
}

void screen_print_int(int n) {
    printf("%d", n);
}