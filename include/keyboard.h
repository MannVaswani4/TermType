#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_enable_raw(void);
void keyboard_disable_raw(void);
int keyboard_keypressed(void);
/* Reads one line into buf (max len-1 chars), returns number of chars read.
   Space is ignored if no chars have been typed in the current word. */
int keyboard_readline(char *buf, int len);

/* Same as keyboard_readline but:
   - records start time on the FIRST keypress via gettimeofday
   - records end time when the line is complete
   - writes elapsed microseconds into *elapsed_us */
int keyboard_readline_timed(char *buf, int len, long long *elapsed_us);

#endif
