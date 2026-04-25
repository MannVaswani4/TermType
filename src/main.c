#include "keyboard.h"
#include "mystring.h"
#include "math.h"
#include "screen.h"
#include "memory.h"
#include <unistd.h>

static unsigned int seed = 12345;

int my_rand() {
    seed = (seed * 1103515245 + 12345);
    return (seed >> 16) & 0x7FFF;
}

int main() {
    screen_print("\033[?25l"); // hide cursor
    while (1) {
        my_reset();

        char *sentence = generate_sentence(5);
        int len = my_strlen(sentence);

        int *state = (int *)my_alloc(sizeof(int) * len);
        int *overflow_lens = (int *)my_alloc(sizeof(int) * len);
        char *overflows = (char *)my_alloc(sizeof(char) * len * 50);
        
        for (int i = 0; i < len; i++) {
            state[i] = 0;
            overflow_lens[i] = 0;
        }

        int pos = 0;

        keyboard_enable_raw();
        screen_clear();

        screen_move(1, 1);
        screen_println("  _______                   _____                 ");
        screen_println(" |__   __|                 |_   _|                ");
        screen_println("    | | ___ _ __ _ __ ___    | | _   _ _ __   ___ ");
        screen_println("    | |/ _ \\ '__| '_ ` _ \\   | || | | | '_ \\ / _ \\");
        screen_println("    | |  __/ |  | | | | | |  | || |_| | |_) |  __/");
        screen_println("    |_|\\___|_|  |_| |_| |_|  \\_/ \\__, | .__/ \\___|");
        screen_println("                                  __/ | |         ");
        screen_println("                                 |___/|_|         ");

        while (1) {
            int word_start = pos;
            if (word_start == len && word_start > 0) word_start--;
            while (word_start > 0 && sentence[word_start - 1] != ' ') {
                word_start--;
            }
            int word_end = word_start;
            while (word_end < len && sentence[word_end] != ' ') {
                word_end++;
            }

            screen_move(10, 1);
            int bottom_printed = 0;
            for (int i = 0; i < len; i++) {
                int draw_state = state[i];
                if (draw_state == 0 && i >= word_start && i < word_end) {
                    draw_state = 2; // purplish
                }

                int underline = 0;
                if (i == pos && sentence[pos] != ' ') {
                    underline = 1;
                }

                screen_print_char_colored(sentence[i], draw_state, underline);
                bottom_printed++;
                
                if (sentence[i] != ' ' && (i == len - 1 || sentence[i+1] == ' ')) {
                    int ws = i;
                    while (ws > 0 && sentence[ws - 1] != ' ') {
                        ws--;
                    }
                    if (overflow_lens[ws] > 0) {
                        for (int j = 0; j < overflow_lens[ws]; j++) {
                            screen_print_char_colored(overflows[ws * 50 + j], -2, 0);
                            bottom_printed++;
                        }
                    }
                }
            }
            for (int i = bottom_printed; i < 150; i++) {
                screen_print(" ");
            }

            if (pos >= len) {
                break;
            }

            int c = keyboard_keypressed();
            if (c == -1) {
                usleep(10000);
                continue;
            }

            if (c == 3 || c == 27) { // Ctrl+C or ESC
                keyboard_disable_raw();
                screen_clear();
                screen_print("\033[?25h"); // show cursor
                return 0;
            }

            if (c == 127 || c == 8) { // BACKSPACE
                if (overflow_lens[word_start] > 0) {
                    overflow_lens[word_start]--;
                } else if (pos > 0) {
                    pos--;
                    state[pos] = 0;
                }
            } else if (c == ' ') { // SPACE
                while (pos < len && sentence[pos] != ' ') {
                    state[pos] = -1;
                    pos++;
                }
                if (pos < len && sentence[pos] == ' ') {
                    state[pos] = 1;
                    pos++;
                }
            } else { // NORMAL KEY
                if (pos < len && sentence[pos] != ' ') {
                    if (c == sentence[pos]) {
                        state[pos] = 1;
                    } else {
                        state[pos] = -1;
                    }
                    pos++;
                } else { // OVERFLOW
                    if (overflow_lens[word_start] < 49) {
                        overflows[word_start * 50 + overflow_lens[word_start]] = (char)c;
                        overflow_lens[word_start]++;
                    }
                }
            }
        }

        keyboard_disable_raw();

        screen_move(12, 1);
        screen_println("Finished! Press 'r' to retry or 'q' to quit.");

        keyboard_enable_raw();
        char choice = 0;
        while (1) {
            int c = keyboard_keypressed();
            if (c == 'q' || c == 'r' || c == 3 || c == 27) {
                choice = c;
                break;
            }
            usleep(10000);
        }
        keyboard_disable_raw();

        if (choice == 'q' || choice == 3 || choice == 27) {
            break;
        }
    }

    screen_clear();
    screen_println("Exiting...");
    screen_print("\033[?25h"); // show cursor
    return 0;
}