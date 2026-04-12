#include "screen.h"
#include "mystring.h"

#include <unistd.h>

/* ── helpers ───────────────────────────────────────────── */

static void write_str(const char *s) {
    write(STDOUT_FILENO, s, my_strlen(s));
}

static void write_ch(char c) {
    write(STDOUT_FILENO, &c, 1);
}

/* Write a non-negative integer directly (no itoa dependency) */
static void write_uint(int n) {
    if (n == 0) { write_ch('0'); return; }
    char tmp[12]; int len = 0;
    while (n > 0) { tmp[len++] = '0' + (n % 10); n /= 10; }
    for (int i = len - 1; i >= 0; i--) write_ch(tmp[i]);
}

/* ── public API ─────────────────────────────────────────── */

void screen_clear(void) {
    write_str("\033[2J\033[H");
}

void screen_cursor_hide(void) { write_str("\033[?25l"); }
void screen_cursor_show(void) { write_str("\033[?25h"); }

void screen_move(int r, int c) {
    write_str("\033[");
    write_uint(r);
    write_ch(';');
    write_uint(c);
    write_ch('H');
}

void screen_print(const char *s) {
    write_str(s);
}

void screen_println(const char *s) {
    write_str(s);
    write_ch('\n');
}

void screen_print_int(int n) {
    if (n < 0) { write_ch('-'); n = -n; }
    write_uint(n);
}

void screen_print_char_colored(char ch, int state) {
    if (state == 1) {
        write_str("\033[32m");   /* green  */
    } else if (state == -1) {
        write_str("\033[31m");   /* red    */
    } else {
        write_str("\033[2;37m"); /* dim white (untyped) */
    }
    write_ch(ch);
    write_str("\033[0m");        /* reset  */
}