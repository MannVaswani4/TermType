#include "keyboard.h"
#include "mystring.h"
#include "math.h"
#include "screen.h"
#include "memory.h"

#include <stdlib.h>



int main() {
    char input[200];

    srand(1);

    char *sentence = generate_sentence(5);

    int len = my_strlen(sentence);

    screen_println("Type this:");
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

    return 0;
}