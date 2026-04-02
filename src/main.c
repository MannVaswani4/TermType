#include "keyboard.h"
#include "mystring.h"
#include "math.h"
#include "screen.h"
#include "memory.h"
#include <unistd.h>
#include <sys/time.h>   /* gettimeofday – only timing header allowed */
#include <stdlib.h>     /* srand/rand for generate_sentence */

/* ── Custom LCG RNG (no rand() dependency for my_rand) ── */
static unsigned int seed = 12345;

static int my_rand(void) {
    seed = seed * 1103515245u + 12345u;
    return (int)((seed >> 16) & 0x7FFF);
}

int main(void) {
    /* Seed both our custom RNG and stdlib rand via gettimeofday */
    struct timeval tv_seed;
    gettimeofday(&tv_seed, 0);
    seed = (unsigned int)(tv_seed.tv_sec ^ tv_seed.tv_usec);
    srand(seed);
    (void)my_rand; /* suppress unused-function warning */

    screen_print("\033[?25l"); /* hide cursor */

    while (1) {
        my_reset();

        char *sentence = generate_sentence(5);
        int len = my_strlen(sentence);

        int *state        = (int  *)my_alloc(sizeof(int)  * len);
        int *overflow_lens = (int  *)my_alloc(sizeof(int)  * len);
        char *overflows   = (char *)my_alloc(sizeof(char) * len * 50);

        int i;
        for (i = 0; i < len; i++) {
            state[i]        = 0;
            overflow_lens[i] = 0;
        }

        int pos = 0;

        /* ── Timing (Task 1) ── */
        int  timing_started = 0;
        struct timeval t_start, t_end;

        /* ── Keystroke counters (Task 1) ── */
        int total_keystrokes   = 0;
        int correct_keystrokes = 0;

        keyboard_enable_raw();
        screen_clear();

        /* ASCII art title with boundary boxes */
        screen_move(1, 1);
        screen_println("\x1b[90m────────────────────────────────────────────────────────────────────────────────\x1b[0m");
        screen_println("\x1b[36m  _______                   _____                 \x1b[0m");
        screen_println("\x1b[36m |__   __|                 |_   _|                \x1b[0m");
        screen_println("\x1b[36m    | | ___ _ __ _ __ ___    | | _   _ _ __   ___ \x1b[0m");
        screen_println("\x1b[36m    | |/ _ \\ '__| '_ ` _ \\   | || | | | '_ \\ / _ \\\x1b[0m");
        screen_println("\x1b[36m    | |  __/ |  | | | | | |  | || |_| | |_) |  __/\x1b[0m");
        screen_println("\x1b[36m    |_|\\___|_|  |_| |_| |_|  \\_/ \\__, | .__/ \\___|\x1b[0m");
        screen_println("\x1b[36m                                  __/ | |         \x1b[0m");
        screen_println("\x1b[36m                                 |___/|_|         \x1b[0m");
        screen_println("\x1b[90m────────────────────────────────────────────────────────────────────────────────\x1b[0m");

        /* ── Real-time typing loop ── */
        while (1) {
            /* Determine current word boundaries */
            int word_start = pos;
            if (word_start == len && word_start > 0) word_start--;
            while (word_start > 0 && sentence[word_start - 1] != ' ') word_start--;
            int word_end = word_start;
            while (word_end < len && sentence[word_end] != ' ')       word_end++;

            /* Render sentence row */
            screen_move(10, 1);
            int bottom_printed = 0;
            for (i = 0; i < len; i++) {
                int draw_state = state[i];
                if (draw_state == 0 && i >= word_start && i < word_end)
                    draw_state = 2; /* purple – current word */

                int underline = (i == pos && pos < len && sentence[pos] != ' ') ? 1 : 0;
                screen_print_char_colored(sentence[i], draw_state, underline);
                bottom_printed++;

                /* Render overflow chars after the last letter of each word */
                if (sentence[i] != ' ' && (i == len - 1 || sentence[i + 1] == ' ')) {
                    int ws = i;
                    while (ws > 0 && sentence[ws - 1] != ' ') ws--;
                    int j;
                    for (j = 0; j < overflow_lens[ws]; j++) {
                        screen_print_char_colored(overflows[ws * 50 + j], -2, 0);
                        bottom_printed++;
                    }
                }
            }
            /* Erase trailing junk */
            for (i = bottom_printed; i < 150; i++) screen_print(" ");

            if (pos >= len) break; /* sentence complete */

            int c = keyboard_keypressed();
            if (c == -1) { usleep(10000); continue; }

            /* Start timer on very first non-control keypress (Task 1) */
            if (!timing_started && c != 3 && c != 27) {
                gettimeofday(&t_start, 0);
                timing_started = 1;
            }

            if (c == 3 || c == 27) {               /* Ctrl-C / ESC – quit */
                keyboard_disable_raw();
                screen_clear();
                screen_print("\033[?25h");
                return 0;
            }

            if (c == 127 || c == 8) {              /* BACKSPACE */
                if (overflow_lens[word_start] > 0) {
                    overflow_lens[word_start]--;
                } else if (pos > 0) {
                    pos--;
                    state[pos] = 0;
                }
            } else if (c == ' ') {                 /* SPACE */
                /* Task 2 fix: only advance if ≥1 char typed in current word */
                int word_has_chars = 0;
                for (i = word_start; i < word_end; i++) {
                    if (state[i] != 0) { word_has_chars = 1; break; }
                }
                if (word_has_chars) {
                    /* Mark remaining untyped chars in word as wrong */
                    while (pos < len && sentence[pos] != ' ') {
                        state[pos] = -1;
                        pos++;
                    }
                    /* Consume the space character */
                    if (pos < len && sentence[pos] == ' ') {
                        state[pos] = 1;
                        pos++;
                    }
                }
                /* else: ignore space silently */
            } else {                               /* NORMAL CHARACTER */
                total_keystrokes++;
                if (pos < len && sentence[pos] != ' ') {
                    if (c == sentence[pos]) {
                        state[pos] = 1;
                        correct_keystrokes++;
                    } else {
                        state[pos] = -1;
                    }
                    pos++;
                } else {                           /* OVERFLOW – past word end */
                    if (overflow_lens[word_start] < 49) {
                        overflows[word_start * 50 + overflow_lens[word_start]] = (char)c;
                        overflow_lens[word_start]++;
                    }
                }
            }
        } /* end typing loop */

        /* ── Record end time ── */
        gettimeofday(&t_end, 0);
        if (!timing_started) t_start = t_end; /* edge case: nothing typed */

        keyboard_disable_raw();

        /* ── Compute stats (Task 1) ── */
        long long elapsed_us = (long long)(t_end.tv_sec  - t_start.tv_sec)  * 1000000LL
                             + (long long)(t_end.tv_usec - t_start.tv_usec);
        if (elapsed_us <= 0) elapsed_us = 1;

        /* Count correct chars from final state array */
        int correct_count = 0;
        for (i = 0; i < len; i++)
            if (state[i] == 1) correct_count++;

        /* WPM = (total_chars / 5) / minutes = total_chars * 12000000 / elapsed_us */
        int wpm = (int)((long long)len * 12000000LL / elapsed_us);

        /* Accuracy = correct_count * 100 / len */
        int accuracy = my_divide(my_multiply(correct_count, 100), len);

        /* ── Worst Keys (Task 3) ── */
        int char_total[256];
        int char_correct[256];
        for (i = 0; i < 256; i++) {
            char_total[i] = 0;
            char_correct[i] = 0;
        }
        for (i = 0; i < len; i++) {
            unsigned char expected = (unsigned char)sentence[i];
            if (expected != ' ') {
                char_total[expected]++;
                if (state[i] == 1) {
                    char_correct[expected]++;
                }
            }
        }

        int worst_chars[256];
        int worst_acc[256];
        int worst_count = 0;
        for (i = 0; i < 256; i++) {
            if (char_total[i] > 0) {
                worst_chars[worst_count] = i;
                worst_acc[worst_count] = my_divide(my_multiply(char_correct[i], 100), char_total[i]);
                worst_count++;
            }
        }

        /* Sort worst_count items by worst_acc (ascending) */
        int j;
        for (i = 0; i < worst_count - 1; i++) {
            for (j = i + 1; j < worst_count; j++) {
                if (worst_acc[j] < worst_acc[i]) {
                    int tmp_acc = worst_acc[i]; worst_acc[i] = worst_acc[j]; worst_acc[j] = tmp_acc;
                    int tmp_c = worst_chars[i]; worst_chars[i] = worst_chars[j]; worst_chars[j] = tmp_c;
                }
            }
        }

        /* ── Display stats overlay (Boxed format) ── */
        screen_move(12, 1);
        screen_println("\x1b[90m────────────────────────────────────────────────────────────────────────────────\x1b[0m");
        screen_println("\x1b[1;37m                                 OVERVIEW                                       \x1b[0m");
        screen_println("\x1b[90m────────────────────────────────────────────────────────────────────────────────\x1b[0m");
        screen_println("");
        screen_print("    \x1b[32mWPM:\x1b[0m                "); screen_print_int(wpm); screen_println("");
        screen_print("    \x1b[33mAccuracy:\x1b[0m           "); screen_print_int(accuracy); screen_println("%");
        screen_print("    \x1b[34mCorrect Keystrokes:\x1b[0m ");
        screen_print_int(correct_keystrokes);
        screen_print(" / ");
        screen_print_int(total_keystrokes);
        screen_println("");
        
        if (worst_count > 0) {
            screen_println("");
            screen_println("    \x1b[1;31mWorst Keys:\x1b[0m");
            int limit = worst_count < 5 ? worst_count : 5;
            for (i = 0; i < limit; i++) {
                screen_print("      '");
                char buf[2] = {(char)worst_chars[i], '\0'};
                screen_print(buf);
                screen_print("': ");
                screen_print_int(worst_acc[i]);
                screen_println("% accuracy");
            }
        }

        screen_println("");
        screen_println("\x1b[90m────────────────────────────────────────────────────────────────────────────────\x1b[0m");
        screen_println("");
        screen_println("    Press \x1b[32m'r'\x1b[0m to retry or \x1b[31m'q'\x1b[0m to quit.");

        /* ── Wait for retry / quit ── */
        keyboard_enable_raw();
        char choice = 0;
        while (1) {
            int c = keyboard_keypressed();
            if (c == 'q' || c == 'r' || c == 3 || c == 27) {
                choice = (char)c;
                break;
            }
            usleep(10000);
        }
        keyboard_disable_raw();

        if (choice == 'q' || choice == 3 || choice == 27) break;
    }

    screen_clear();
    screen_println("Exiting...");
    screen_print("\033[?25h"); /* show cursor */
    return 0;
}
