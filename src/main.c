#include "keyboard.h"
#include "mystring.h"
#include "math.h"
#include "screen.h"
#include "memory.h"

static unsigned int seed = 12345;

int my_rand() {
    seed = (seed * 1103515245 + 12345);
    return (seed >> 16) & 0x7FFF;
}

int main() {
    char input[200];
    char choice[10];

    while (1) {

        my_reset();  // important

        char *sentence = generate_sentence(5);
        int len = my_strlen(sentence);

        screen_println("\nType this:");
        screen_println(sentence);
        screen_println("");

        screen_print("Your input: ");
        keyboard_readline(input, 200);

        int errors = 0;

        for (int i = 0; i < len; i++) {
            if (input[i] != sentence[i]) {
                errors++;
            }
        }

        int correct = len - errors;

        int accuracy = my_divide(
            my_multiply(correct, 100),
            len
        );

        screen_println("\n");

        screen_print("Accuracy: ");
        screen_print_int(accuracy);
        screen_println("%");

        screen_print("Errors: ");
        screen_print_int(errors);
        screen_println("");

        screen_print("\nPress 'r' to retry or 'q' to quit: ");
        keyboard_readline(choice, 10);

        if (choice[0] == 'q') {
            break;
        }
    }

    screen_println("\nExiting...");

    return 0;
}