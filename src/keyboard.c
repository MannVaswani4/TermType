#include "keyboard.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>

static struct termios orig_termios;
static int raw_active = 0;

/* ── Raw mode (used by the real-time game loop) ── */

void keyboard_enable_raw(void) {
    if (raw_active) return;
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= (unsigned int)~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    raw_active = 1;
}

void keyboard_disable_raw(void) {
    if (!raw_active) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    raw_active = 0;
}

/* Non-blocking: returns the keycode, or -1 if nothing available */
int keyboard_keypressed(void) {
    unsigned char c = 0;
    if (read(STDIN_FILENO, &c, 1) == 1) return (int)c;
    return -1;
}

/* ── Blocking readline (cooked mode, with space guard – Task 2) ── */

int keyboard_readline(char *buf, int len) {
    int i = 0;
    int word_chars = 0;
    int c;

    if (!buf || len <= 0) return 0;

    while (i < len - 1) {
        c = getchar();
        if (c == EOF || c == '\n') break;

        /* Space guard: ignore space if no chars typed in current word */
        if (c == ' ') {
            if (word_chars == 0) continue;
            word_chars = 0;
        } else {
            word_chars++;
        }
        buf[i++] = (char)c;
    }
    buf[i] = '\0';
    return i;
}

/* ── Timed readline (cooked mode, space guard + gettimeofday – Task 1) ── */

int keyboard_readline_timed(char *buf, int len, long long *elapsed_us) {
    int i = 0;
    int word_chars = 0;
    int started = 0;
    int c;
    struct timeval t_start, t_end;

    if (!buf || len <= 0 || !elapsed_us) return 0;
    *elapsed_us = 1;

    while (i < len - 1) {
        c = getchar();
        if (c == EOF || c == '\n') break;

        if (!started) {
            gettimeofday(&t_start, 0);
            started = 1;
        }

        if (c == ' ') {
            if (word_chars == 0) continue;
            word_chars = 0;
        } else {
            word_chars++;
        }
        buf[i++] = (char)c;
    }

    if (started) {
        gettimeofday(&t_end, 0);
        *elapsed_us = (long long)(t_end.tv_sec  - t_start.tv_sec)  * 1000000LL
                    + (long long)(t_end.tv_usec - t_start.tv_usec);
        if (*elapsed_us <= 0) *elapsed_us = 1;
    }

    buf[i] = '\0';
    return i;
}
