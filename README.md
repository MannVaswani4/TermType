# TermType

TermType is a terminal-based typing test written entirely in C, built from the ground up without relying on the standard C library. There is no `<string.h>`, no `<math.h>`, and no `malloc()` or `free()`. Every subsystem — memory allocation, string handling, arithmetic, screen output, and keyboard input — is implemented from scratch using only low-level POSIX system calls.

![TermType screenshot](./utils/Screenshot%202026-04-28%20at%205.56.56%20AM.png)

---

## Features

- WPM, Accuracy, Keystroke tracking, Worst Keys analysis
- Custom word count via CLI argument
- Zero standard libraries — every utility is hand-rolled:
  - `memory.c` — Static 1 MB memory pool with a bump allocator; no `malloc`
  - `math.c` — Safe integer multiply and divide
  - `string.c` — String utilities and sentence generation
  - `screen.c` — ANSI escape sequences, box drawing, and color rendering
  - `keyboard.c` — Raw terminal mode via `termios`, non-blocking character input

---

## Build

```bash
git clone https://github.com/MannVaswani4/TermType.git
cd TermType

gcc src/*.c -I include/ -o termtype
```

---

## Usage

```bash
./termtype          # default: 50 words
./termtype 25       # 25 words
./termtype 100      # 100 words
```

The argument must be an integer between 1 and 1000.

---

## Project Structure

```
TermType/
├── include/
│   ├── keyboard.h
│   ├── math.h
│   ├── memory.h
│   ├── mystring.h
│   ├── screen.h
│   └── words.h
├── src/
│   ├── main.c       — application loop, state machine, timing, stats
│   ├── keyboard.c   — raw terminal mode, non-blocking input
│   ├── screen.c     — ANSI rendering, frame buffer, box drawing
│   ├── memory.c     — static memory pool allocator
│   ├── string.c     — string utilities
│   ├── math.c       — integer arithmetic
│   └── words.c      — word bank and sentence generator
└── utils/
    └── screenshot.png
```
