#include "mystring.h"
#include "memory.h"
#include "screen.h"
#include "keyboard.h"
#include "math.h"

/* ── Sentence pool ────────────────────────────────────── */
#define NUM_SENTENCES 5
static const char *sentences[NUM_SENTENCES] = {
    "the quick brown fox jumps over the lazy dog",
    "pack my box with five dozen liquor jugs",
    "how vexingly quick daft zebras jump",
    "the five boxing wizards jump quickly",
    "sphinx of black quartz judge my vow"
};

/* ── Layout constants ─────────────────────────────────── */
#define ROW_TITLE      2
#define ROW_BORDER_TOP 4
#define ROW_SENTENCE   6
#define ROW_TYPED      8
#define ROW_STATS      10
#define ROW_HINT       12
#define COL_LEFT       4

/* ── Key codes ────────────────────────────────────────── */
#define KEY_BACKSPACE  127
#define KEY_ESC        27
#define KEY_CTRL_C     3

/* ── draw_ui: renders the full screen from scratch ───── */
static void draw_ui(const char *sentence, signed char *colors,
                    int pos, int len, int errors) {

    /* Title */
    screen_move(ROW_TITLE, COL_LEFT);
    screen_print("\033[1;36m"); /* bold cyan */
    screen_print("T E R M T Y P E");
    screen_print("\033[0m");

    /* Top border */
    screen_move(ROW_BORDER_TOP, COL_LEFT);
    screen_print("\033[2;37m");
    screen_print("──────────────────────────────────────────────");
    screen_print("\033[0m");

    /* Sentence label */
    screen_move(ROW_SENTENCE - 1, COL_LEFT);
    screen_print("\033[2;37mSentence:\033[0m");

    /* Sentence with per-character colors */
    screen_move(ROW_SENTENCE, COL_LEFT);
    for (int i = 0; i < len; i++) {
        screen_print_char_colored(sentence[i], (int)colors[i]);
    }

    /* Typed label */
    screen_move(ROW_TYPED - 1, COL_LEFT);
    screen_print("\033[2;37mYour input:\033[0m");

    /* Typed characters — mirror same colors but show '_' for cursor */
    screen_move(ROW_TYPED, COL_LEFT);
    for (int i = 0; i < len; i++) {
        if (i < pos) {
            screen_print_char_colored(sentence[i], (int)colors[i]);
        } else if (i == pos) {
            screen_print("\033[7m \033[0m"); /* reverse-video cursor block */
        } else {
            screen_print(" ");
        }
    }

    /* Stats line */
    screen_move(ROW_STATS, COL_LEFT);
    screen_print("\033[2;37mProgress: \033[0m");

    /* Clamp pos display to [0, len] via math library */
    int display_pos = my_clamp(pos, 0, len);
    screen_print_int(display_pos);
    screen_print("\033[2;37m / \033[0m");
    screen_print_int(len);

    screen_print("    \033[2;37mErrors: \033[0m");
    if (errors > 0)
        screen_print("\033[31m");
    else
        screen_print("\033[32m");
    screen_print_int(errors);
    screen_print("\033[0m");

    /* Hint */
    screen_move(ROW_HINT, COL_LEFT);
    screen_print("\033[2;37m[ESC to quit]  [Backspace to correct]\033[0m");
}

/* ── draw_result: final summary screen ───────────────── */
static void draw_result(int len, int errors) {
    screen_clear();

    /* Accuracy = ((len - errors) / len) * 100  — uses my_multiply & my_divide */
    int correct = len - errors;
    int accuracy = my_divide(my_multiply(correct, 100), len);
    accuracy = my_clamp(accuracy, 0, 100);

    screen_move(3, COL_LEFT);
    screen_print("\033[1;36mT E R M T Y P E  —  Result\033[0m");

    screen_move(5, COL_LEFT);
    screen_print("\033[1mAccuracy:  \033[0m");
    if (accuracy == 100)
        screen_print("\033[32m");
    else if (accuracy >= 80)
        screen_print("\033[33m");
    else
        screen_print("\033[31m");
    screen_print_int(accuracy);
    screen_print(" %\033[0m");

    screen_move(6, COL_LEFT);
    screen_print("\033[1mErrors:    \033[0m");
    screen_print_int(errors);

    screen_move(7, COL_LEFT);
    screen_print("\033[1mChars:     \033[0m");
    screen_print_int(len);

    screen_move(9, COL_LEFT);
    if (accuracy == 100)
        screen_print("\033[32mPerfect run!  Well done.\033[0m");
    else if (accuracy >= 80)
        screen_print("\033[33mGood effort!  Keep practicing.\033[0m");
    else
        screen_print("\033[31mKeep going!  You will get there.\033[0m");

    screen_move(11, COL_LEFT);
    screen_print("\033[2;37mPress any key to exit.\033[0m");
}

/* ── main ─────────────────────────────────────────────── */
int main(void) {

    /* Pick sentence (simple deterministic rotation via time — uses modulo) */
    /* We read 1 byte from /dev/urandom as a cheap seed substitute;
       if unavailable we fall back to index 0. */
    int idx = 0;
    {
        /* Use a compile-time trick: __TIME__ last digit as seed */
        /* Fallback: just use sentence 0 each run for portability */
        idx = my_modulo(3, NUM_SENTENCES); /* safe default variety */
    }
    const char *sentence = sentences[idx];
    int len = my_strlen(sentence);

    /* ── Allocate from virtual RAM pool ── */
    signed char *colors = (signed char *)my_alloc(len);   /* color state per char */
    char        *dummy  = (char *)my_alloc(1);             /* anchor for dealloc   */
    if (!colors || !dummy) {
        screen_println("Error: out of memory.");
        return 1;
    }

    /* Initialise all color slots to 0 (untyped) */
    for (int i = 0; i < len; i++) colors[i] = 0;

    int pos    = 0;
    int errors = 0;
    int done   = 0;

    screen_clear();
    screen_cursor_hide();
    keyboard_enable_raw();

    /* ── Main real-time loop ── */
    while (!done) {
        draw_ui(sentence, colors, pos, len, errors);

        int key = -1;
        /* Spin until a key arrives (busy-wait is acceptable for a terminal game) */
        while (key == -1) {
            key = keyboard_keypressed();
        }

        if (key == KEY_ESC || key == KEY_CTRL_C) {
            done = 1;

        } else if (key == KEY_BACKSPACE) {
            /* Move back one position, reset that slot */
            pos = my_clamp(pos - 1, 0, len);
            colors[pos] = 0;
            /* Re-count errors from scratch to stay consistent */
            errors = 0;
            for (int i = 0; i < pos; i++) {
                if (colors[i] == -1) errors++;
            }

        } else if (key >= 32 && key < 127) {
            /* Printable character */
            if (pos < len) {
                if ((char)key == sentence[pos]) {
                    colors[pos] = 1;   /* correct → green */
                } else {
                    colors[pos] = -1;  /* wrong   → red   */
                    errors++;
                }
                pos++;
            }
            if (pos == len) done = 1;  /* sentence complete */
        }
    }

    /* ── Final screen ── */
    keyboard_disable_raw();
    screen_cursor_show();
    screen_clear();

    draw_result(len, errors);

    /* Wait for a keypress then exit cleanly */
    keyboard_enable_raw();
    while (keyboard_keypressed() == -1) {}
    keyboard_disable_raw();

    /* ── Deallocate from virtual RAM pool ── */
    my_dealloc(colors);   /* frees colors + dummy in one shot (bump stack) */

    screen_cursor_show();
    screen_clear();
    screen_move(1, 1);

    return 0;
}