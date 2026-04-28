#include "keyboard.h"
#include "mystring.h"
#include "math.h"
#include "screen.h"
#include "memory.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>   /* gettimeofday – only timing header allowed */
#include <stdlib.h>     /* srand/rand for generate_sentence */

/* ── Custom LCG RNG (no rand() dependency for my_rand) ── */
static unsigned int seed = 12345;

static int my_rand(void) {
    seed = seed * 1103515245u + 12345u;
    return (int)((seed >> 16) & 0x7FFF);
}

int main(int argc, char *argv[]) {
    /* ── Parse optional word count from CLI ── */
    int num_words = 50; /* default */

    if (argc > 1) {
        int parsed = 0;
        int k;
        for (k = 0; argv[1][k] != '\0'; k++) {
            char ch = argv[1][k];
            if (ch < '0' || ch > '9') {
                screen_print("Invalid word count. Enter a number between 1 and 1000.\n");
                return 1;
            }
            parsed = parsed * 10 + (ch - '0');
        }
        if (k == 0 || parsed < 1 || parsed > 1000) {
            screen_print("Invalid word count. Enter a number between 1 and 1000.\n");
            return 1;
        }
        num_words = parsed;
    }

    /* Seed both our custom RNG and stdlib rand via gettimeofday */
    struct timeval tv_seed;
    gettimeofday(&tv_seed, 0);
    seed = (unsigned int)(tv_seed.tv_sec ^ tv_seed.tv_usec);
    srand(seed);
    (void)my_rand; /* suppress unused-function warning */

    screen_print("\033[?25l"); /* hide cursor */

    while (1) {
        my_reset();

        char *sentence = generate_sentence(num_words);
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

        /* ── Timing ── */
        int  timing_started = 0;
        struct timeval t_start, t_end;

        /* ── Keystroke counters ── */
        /* Every physical key press is classified correct or wrong */
        int total_keystrokes   = 0;   /* every key pressed (incl backspace, overflow) */
        int correct_keystrokes = 0;

        /* Per-character key accuracy (for worst keys): only actual letter presses */
        /* key_total[c] = times key 'c' was physically pressed as a normal char    */
        /* key_correct[c] = times it was correct                                    */
        int key_total[256];
        int key_correct[256];
        for (i = 0; i < 256; i++) { key_total[i] = 0; key_correct[i] = 0; }

        keyboard_enable_raw();
        screen_clear();

        /* ASCII art title */
        screen_move(1, 1);
        screen_println("");
        screen_println("\x1b[36m  _______                   _____                 \x1b[0m");
        screen_println("\x1b[36m |__   __|                 |_   _|                \x1b[0m");
        screen_println("\x1b[36m    | | ___ _ __ _ __ ___    | | _   _ _ __   ___ \x1b[0m");
        screen_println("\x1b[36m    | |/ _ \\ '__| '_ ` _ \\   | || | | | '_ \\ / _ \\\x1b[0m");
        screen_println("\x1b[36m    | |  __/ |  | | | | | |  | || |_| | |_) |  __/\x1b[0m");
        screen_println("\x1b[36m    |_|\\___|_|  |_| |_| |_|  \\_/ \\__, | .__/ \\___|\x1b[0m");
        screen_println("\x1b[36m                                  __/ | |         \x1b[0m");
        screen_println("\x1b[36m                                 |___/|_|         \x1b[0m");
        screen_println("");
        screen_println("");

        int final_text_row = 15;

        /* ── Real-time typing loop ── */
        while (1) {
            /* Determine current word boundaries (only meaningful while typing) */
            int word_start = 0, word_end = 0;
            if (pos < len) {
                word_start = pos;
                while (word_start > 0 && sentence[word_start - 1] != ' ') word_start--;
                word_end = word_start;
                while (word_end < len && sentence[word_end] != ' ')       word_end++;
            }

            struct winsize ws;
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
            int term_cols = ws.ws_col;
            if (term_cols < 20) term_cols = 80;

            /* Render top border for text box */
            screen_move(11, 1);
            screen_print("\x1b[2K");
            screen_print("\x1b[90m\u250c");
            for (int c = 2; c < term_cols; c++) screen_print("\u2500");
            screen_print("\u2510\x1b[0m");

            int text_row = 12;
            int text_col = 3;
            screen_move(text_row, 1);
            screen_print("\x1b[2K");
            screen_print("\x1b[90m\u2502\x1b[0m");
            screen_move(text_row, term_cols);
            screen_print("\x1b[90m\u2502\x1b[0m");
            screen_move(text_row, text_col);

            for (i = 0; i < len; i++) {
                int draw_state = state[i];
                if (draw_state == 0 && i >= word_start && i < word_end)
                    draw_state = 2; /* purple – current word */

                int underline = (i == pos && pos < len && sentence[pos] != ' ') ? 1 : 0;

                if (text_col >= term_cols - 1) {
                    text_row++;
                    text_col = 3;
                    screen_move(text_row, 1);
                    screen_print("\x1b[2K");
                    screen_print("\x1b[90m\u2502\x1b[0m");
                    screen_move(text_row, term_cols);
                    screen_print("\x1b[90m\u2502\x1b[0m");
                    screen_move(text_row, text_col);
                }
                screen_print_char_colored(sentence[i], draw_state, underline);
                text_col++;

                /* Render overflow chars after the last letter of each word */
                if (sentence[i] != ' ' && (i == len - 1 || sentence[i + 1] == ' ')) {
                    int ws_idx = i;
                    while (ws_idx > 0 && sentence[ws_idx - 1] != ' ') ws_idx--;
                    int j;
                    for (j = 0; j < overflow_lens[ws_idx]; j++) {
                        if (text_col >= term_cols - 1) {
                            text_row++;
                            text_col = 3;
                            screen_move(text_row, 1);
                            screen_print("\x1b[2K");
                            screen_print("\x1b[90m\u2502\x1b[0m");
                            screen_move(text_row, term_cols);
                            screen_print("\x1b[90m\u2502\x1b[0m");
                            screen_move(text_row, text_col);
                        }
                        screen_print_char_colored(overflows[ws_idx * 50 + j], -2, 0);
                        text_col++;
                    }
                }
            }
            
            /* Render bottom border */
            text_row++;
            screen_move(text_row, 1);
            screen_print("\x1b[2K");
            screen_print("\x1b[90m\u2514");
            for (int c = 2; c < term_cols; c++) screen_print("\u2500");
            screen_print("\u2518\x1b[0m");

            /* Erase trailing junk rows (in case text shrank) */
            for (int r = text_row + 1; r < text_row + 4; r++) {
                screen_move(r, 1);
                screen_print("\x1b[2K");
            }

            final_text_row = text_row;

            screen_flush(); /* push entire frame in one write() */

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
                    /* Deleting an overflow char → correct (it was extra/wrong) */
                    overflow_lens[word_start]--;
                    total_keystrokes++;
                    correct_keystrokes++;
                } else if (pos > 0) {
                    pos--;
                    /* Backspace over wrong char → correct; over correct → wrong */
                    total_keystrokes++;
                    if (state[pos] == -1) {
                        correct_keystrokes++;
                    }
                    state[pos] = 0;
                }
            } else if (c == ' ') {                 /* SPACE */
                /* Count every spacebar press as a keystroke */
                total_keystrokes++;

                /* Space is correct ONLY when: every char of current word is state==1
                   AND there is no overflow for this word */
                int word_complete = (word_end > word_start); /* must have a word */
                if (word_complete) {
                    int wj;
                    for (wj = word_start; wj < word_end; wj++) {
                        if (state[wj] != 1) { word_complete = 0; break; }
                    }
                    if (overflow_lens[word_start] > 0) word_complete = 0;
                }
                if (word_complete) correct_keystrokes++;

                /* Still advance to next word if ≥1 char was typed */
                int word_has_chars = 0;
                for (i = word_start; i < word_end; i++) {
                    if (state[i] != 0) { word_has_chars = 1; break; }
                }
                if (word_has_chars) {
                    while (pos < len && sentence[pos] != ' ') pos++;
                    if (pos < len && sentence[pos] == ' ') {
                        state[pos] = 1;
                        pos++;
                    }
                }
                /* else: wrong space with nothing typed – counted but don't advance */

            } else {                               /* NORMAL CHARACTER */
                total_keystrokes++;
                unsigned char uc = (unsigned char)c;
                if (pos < len && sentence[pos] != ' ') {
                    if (c == sentence[pos]) {
                        state[pos] = 1;
                        correct_keystrokes++;
                        key_total[uc]++;
                        key_correct[uc]++;
                    } else {
                        state[pos] = -1;
                        key_total[uc]++;
                        /* key_correct not incremented – wrong press */
                    }
                    pos++;
                } else {                           /* OVERFLOW – past word end */
                    if (overflow_lens[word_start] < 49) {
                        overflows[word_start * 50 + overflow_lens[word_start]] = (char)c;
                        overflow_lens[word_start]++;
                    }
                    /* overflow keypress = wrong (needed a space, got a letter) */
                    key_total[uc]++;
                }
            }
        } /* end typing loop */

        /* ── Record end time ── */
        gettimeofday(&t_end, 0);
        if (!timing_started) t_start = t_end; /* edge case: nothing typed */

        keyboard_disable_raw();

        /* ── Compute stats ── */
        long long elapsed_us = (long long)(t_end.tv_sec  - t_start.tv_sec)  * 1000000LL
                             + (long long)(t_end.tv_usec - t_start.tv_usec);
        if (elapsed_us <= 0) elapsed_us = 1;

        /* Adjusted WPM: only count words where EVERY character was correct
           (no wrong, no skipped/untyped, no overflow) */
        int correct_words = 0;
        {
            int wi = 0; /* walk through sentence word by word */
            while (wi < len) {
                /* find word boundaries */
                int ws2 = wi;
                int we2 = wi;
                while (we2 < len && sentence[we2] != ' ') we2++;
                /* check: all chars correct, no overflow */
                int word_ok = 1;
                int wj;
                for (wj = ws2; wj < we2; wj++) {
                    if (state[wj] != 1) { word_ok = 0; break; }
                }
                if (word_ok && overflow_lens[ws2] == 0) correct_words++;
                wi = we2 + 1; /* skip past the space */
            }
        }
        /* WPM = correct_words / minutes elapsed */
        int wpm = (int)((long long)correct_words * 60000000LL / elapsed_us);

        /* Accuracy = correct keypresses / total keypresses * 100 */
        int accuracy = (total_keystrokes > 0)
            ? my_divide(my_multiply(correct_keystrokes, 100), total_keystrokes)
            : 100;

        /* ── Worst Keys: based on actual key_total/key_correct arrays ── */
        int worst_chars[256];
        int worst_acc[256];
        int worst_count = 0;
        for (i = 0; i < 256; i++) {
            if (key_total[i] > 0) {
                worst_chars[worst_count] = i;
                worst_acc[worst_count] = my_divide(my_multiply(key_correct[i], 100), key_total[i]);
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

        /* ── Display stats overlay (4-sided box) ── */
        screen_move(final_text_row + 2, 1);
        screen_print("\x1b[2K"); screen_println("\x1b[90m  \u250c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2510\x1b[0m");
        screen_print("\x1b[2K"); screen_print("\x1b[90m  \u2502\x1b[1;37m");
        screen_print("                     OVERVIEW                    ");
        screen_println("\x1b[54G\x1b[90m\u2502\x1b[0m");
        screen_print("\x1b[2K"); screen_println("\x1b[90m  \u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524\x1b[0m");
        screen_print("\x1b[2K"); screen_println("\x1b[90m  \u2502\x1b[54G\u2502\x1b[0m");
        screen_print("\x1b[2K"); screen_print("\x1b[90m  \u2502\x1b[0m    \x1b[32mWPM:\x1b[0m           "); screen_print_int(wpm);
        screen_println("\x1b[54G\x1b[90m\u2502\x1b[0m");
        screen_print("\x1b[2K"); screen_print("\x1b[90m  \u2502\x1b[0m    \x1b[33mAccuracy:\x1b[0m      "); screen_print_int(accuracy); screen_print("%");
        screen_println("\x1b[54G\x1b[90m\u2502\x1b[0m");
        screen_print("\x1b[2K"); screen_print("\x1b[90m  \u2502\x1b[0m    \x1b[34mKeystrokes:\x1b[0m    ");
        screen_print_int(correct_keystrokes); screen_print(" / "); screen_print_int(total_keystrokes);
        screen_println("\x1b[54G\x1b[90m\u2502\x1b[0m");

        if (worst_count > 0) {
            screen_print("\x1b[2K"); screen_println("\x1b[90m  \u2502\x1b[54G\u2502\x1b[0m");
            screen_print("\x1b[2K"); screen_println("\x1b[90m  \u2502\x1b[0m    \x1b[1;31mWorst Keys:\x1b[0m\x1b[54G\x1b[90m\u2502\x1b[0m");
            int limit = worst_count < 5 ? worst_count : 5;
            for (i = 0; i < limit; i++) {
                screen_print("\x1b[2K"); screen_print("\x1b[90m  \u2502\x1b[0m      '");
                char buf[2] = {(char)worst_chars[i], '\0'};
                screen_print(buf); screen_print("': ");
                screen_print_int(worst_acc[i]); screen_print("%");
                screen_println("\x1b[54G\x1b[90m\u2502\x1b[0m");
            }
        }

        screen_print("\x1b[2K"); screen_println("\x1b[90m  \u2502\x1b[54G\u2502\x1b[0m");
        screen_print("\x1b[2K"); screen_println("\x1b[90m  \u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518\x1b[0m");
        screen_print("\x1b[2K"); screen_println("");
        screen_print("\x1b[2K"); screen_println("  \x1b[3mPress \x1b[32m'r'\x1b[0m\x1b[3m to retry or \x1b[31m'q'\x1b[0m\x1b[3m to quit.\x1b[0m");
        screen_flush(); /* flush stats overlay */

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
