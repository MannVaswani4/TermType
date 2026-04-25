#ifndef SCREEN_H
#define SCREEN_H

void screen_print(const char *s);
void screen_println(const char *s);
void screen_print_int(int n);

void screen_move(int row, int col);
void screen_print_char_colored(char c, int state, int underline);
void screen_clear(void);

#endif