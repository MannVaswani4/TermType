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

