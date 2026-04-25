#include "keyboard.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

static struct termios orig_termios;

void keyboard_enable_raw(void) {
    ioctl(STDIN_FILENO, TIOCGETA, &orig_termios);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    
    ioctl(STDIN_FILENO, TIOCSETA, &raw);
}

void keyboard_disable_raw(void) {
    ioctl(STDIN_FILENO, TIOCSETA, &orig_termios);
}

int keyboard_keypressed(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }
    return -1;
}