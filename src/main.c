#include "mystring.h"
#include "memory.h"
#include "screen.h"
#include "keyboard.h"

#define MAX_INPUT 256

int main(void) {
    const char *sentence = "the quick brown fox jumps over the lazy dog";
    /* Allocate input buffer from our custom memory pool */
    char *input = (char *)my_alloc(MAX_INPUT);
    if (!input) {
        screen_println("Error: out of memory.");
        return 1;
    }

    /* --- Display the sentence --- */
    screen_clear();
    screen_println("=== Typing Test ===");
    screen_println("");
    screen_print("Type this: ");
    screen_println(sentence);
    screen_println("");
    screen_print("Your input: ");

    /* --- Read user input --- */
    keyboard_readline(input, MAX_INPUT);

    /* --- Compare and show result --- */
    screen_println("");
    if (my_strcmp(input, sentence) == 0) {
        screen_println("Result: CORRECT! Well done.");
    } else {
        screen_println("Result: INCORRECT. Try again.");
    }

    screen_println("");
    screen_print("You typed:    ");
    screen_println(input);
    screen_print("Expected:     ");
    screen_println(sentence);

    return 0;
}