# TermType ⌨️

TermType is a minimalist, real-time terminal typing tutor written entirely in C from scratch. What makes TermType unique is its strict adherence to low-level systems programming—it operates **without the standard C libraries**. There is no `<string.h>`, no `<math.h>`, and absolutely no standard `malloc()` or `free()`.

Every component, from dynamic memory allocation to string manipulation, math calculations, and terminal manipulation, is powered entirely by custom-built internal libraries.

## 🚀 Features

- **Real-Time Analytics:** Calculates your Words Per Minute (WPM) using raw system syscalls (`<sys/time.h>`) and provides real-time accuracy scoring.
- **Worst Keys Tracking:** Analyzes your typing performance on a per-character basis and isolates your top 5 "worst keys", helping you quickly identify exact weaknesses to practice.
- **Dynamic Terminal UI:** A sleek, fully boxed ASCII interface using raw terminal ANSI escape codes. Features dynamic word highlighting, live error coloring, and responsive cursor movement.
- **Overflow Handling:** Elegantly handles situations where you type too many characters for a specific word, visually tracking and highlighting your overflowing keystrokes in bright red.
- **Zero Standard Libraries:** 
  - **Memory (`memory.c`):** Features a custom 64KB static memory pool with a first-fit allocation and deallocation strategy.
  - **Math (`math.c`):** Custom implementations of safe division, multiplication, clamping, and modulo.
  - **String (`string.c`):** Hand-rolled string length, comparison, and a custom sentence generation engine.
  - **Screen (`screen.c`):** A direct wrapper over Unix `write()` for printing strings, formatted integers, and colored ASCII art directly to the terminal.
  - **Keyboard (`keyboard.c`):** Custom implementations of POSIX raw terminal control (`termios`) enabling non-blocking, instantaneous character-by-character input.

## 🛠️ Build & Run

TermType is compiled using `gcc` with strict flags to ensure no hidden system dependencies creep into the logic.

```bash
# Clone the repository
git clone https://github.com/MannVaswani4/TermType.git
cd TermType

# Compile the project
gcc -Wall -Wextra -pedantic src/*.c -Iinclude -o TermType

# Run the typing tutor
./TermType
```

## 🎮 How to Play

1. Run the `./TermType` executable in your terminal.
2. The timer **will not start** until you press your very first keystroke. Take your time to read the generated sentence!
3. Type the sentence exactly as it appears. 
   - **Green**: Correct characters
   - **Red**: Incorrect characters
   - **Purple**: Your current active word
   - **Underline**: The exact character you are currently typing
4. If you make a mistake, you can seamlessly use `Backspace` to correct it.
5. Upon completion, a comprehensive **Overview** boundary box will appear, displaying your total WPM, Accuracy, Correct Keystrokes ratio, and the **Worst Keys** you struggled with.
6. Press `r` to retry instantly with a newly generated sentence, or `q` to safely quit and restore your terminal cursor.

## 📁 Architecture

The project is structured into strict, separated architectural domains:

- `src/main.c`: The core application loop, game state tracking, and timing logic.
- `src/keyboard.c` / `include/keyboard.h`: Raw mode configurations, terminal state preservation, and non-blocking input handling.
- `src/screen.c` / `include/screen.h`: ANSI escape sequences, box drawing, color rendering, and screen-clearing logic.
- `src/memory.c` / `include/memory.h`: The custom static 64KB free-list memory allocator bypassing the OS `malloc`.
- `src/string.c` / `include/mystring.h`: Low-level character manipulation and word-bank sentence generation.
- `src/math.c` / `include/math.h`: Custom arithmetic logic.

