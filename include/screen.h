#ifndef SCREEN_H
#define SCREEN_H

void screen_clear(void);
void screen_print(const char *s);
void screen_println(const char *s);

/* Position cursor at row r, column c (1-indexed) */
void screen_move(int r, int c);

/* Print a single character with ANSI color:
   state  1 = green (correct)
   state -1 = red   (wrong)
   state  0 = dim   (not yet typed)         */
void screen_print_char_colored(char ch, int state);

/* Print an integer as a string */
void screen_print_int(int n);

/* Hide / show the terminal cursor */
void screen_cursor_hide(void);
void screen_cursor_show(void);

#endif