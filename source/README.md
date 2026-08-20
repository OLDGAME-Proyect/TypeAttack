# Source Architecture Documentation

Technical specification and module breakdown for the Type Attack terminal engine.

---

## 1. System Architecture

The engine is built around a non-blocking poll loop operating at an approximate fixed rate of 60 frames per second (16 ms cycle delay). The architecture separates terminal rendering, input buffer management, sentence generation, and persistent state storage into dedicated compilation units.

```
+-------------------------------------------------------------+
|                          main.c                             |
|      (Initialization, Configuration & State Dispatch)       |
+------------------------------+------------------------------+
                               |
               +---------------+---------------+
               |                               |
               v                               v
      +-----------------+             +-----------------+
      |     menu.c      |             |     game.c      |
      | (Menu Loop & UI)|             | (Core Game Loop)|
      +--------+--------+             +---+----+----+---+
               |                          |    |    |   
               |      +-------------------+    |    +-------------------+
               |      |                        |                        |
               v      v                        v                        v
      +-----------------+             +-----------------+      +-----------------+
      |     score.c     |             |    render.c     |      |     words.c     |
      | (Registry/SHA256|             | (PDCurses Render|      | (Bank Loaders   |
      |   Persistence)  |             |  & FX Pipeline) |      |  & Allocations) |
      +-----------------+             +-----------------+      +-----------------+
```

---

## 2. Module Breakdown

### 2.1 `main.c` / `main.h`
- Entry point for the executable.
- Initializes Curses screen state: `initscr()`, `cbreak()`, `noecho()`, `curs_set(0)`, `nodelay(stdscr, TRUE)`, `keypad(stdscr, TRUE)`.
- Invokes `render_init()` to construct color pairs.
- Invokes `words_init()` to populate memory pools for `words`, `congrats`, and `loser` dictionaries.
- Parses `config` file for difficulty profile (`00` to `03`), writing default configuration if missing.
- Dispatches execution between `menu()` and `game_loop()`.
- Handles clean terminal de-initialization via `words_free()` and `endwin()`.

### 2.2 `menu.c` / `menu.h`
- Implements interactive main menu loop.
- Calculates and displays persistent global level based on stored score:
  $$\text{Global Level} = 1 + \left\lfloor \frac{\text{TotalScore}}{300} \right\rfloor \quad (\text{for } \text{TotalScore} > 0)$$
- Handles navigation input (`KEY_UP`, `KEY_DOWN`, `W`, `S`, `ENTER`, `ESC`).

### 2.3 `game.c` / `game.h`
- Encapsulates the core active game loop.
- **Input Pipeline**: Continuously drains the Curses input queue using a non-blocking `while ((ch = getch()) != ERR)` loop. Handles ASCII printable characters `[32, 126]`, backspace variations (`KEY_BACKSPACE`, `8`, `127`, `\b`), and abort signals (`27` / `ESC`).
- **Kinematics & Speed Scaling**: Calculates fall velocity based on difficulty string, sentence length, word count, and current player session level:
  $$\text{TotalFrames} = \frac{\text{FallTimeMs}}{16}$$
  $$\text{BaseSpeed} = \frac{\text{MinLine} - 2}{\text{TotalFrames}}$$
  $$\text{FallSpeed} = \text{BaseSpeed} \times \left(1.0 + (\text{Level} - 1) \times 0.18\right)$$
- **State Validation**:
  - Success: When `input_len == target_len` and string equality is satisfied, increments `correct_count`, computes level progression, adds `+100` points to persistent storage, dispatches `congrats` event, and pulls a new sentence.
  - Failure: When `s.y >= min_line`, deducts `100` points from persistent storage, dispatches `loser` event, and resets the falling sentence.

### 2.4 `render.c` / `render.h`
- Abstracted drawing layer using PDCurses/NCurses primitives.
- Color Pair Mapping:
  - `Pair 1`: Black on White (Header / High-contrast badges)
  - `Pair 2`: Green on Black (Success events, rainbow stage 1)
  - `Pair 3`: Red on Black (Failure events, mismatched characters)
  - `Pair 4`: Yellow on Black (Status text, rainbow stage 2)
  - `Pair 5`: Cyan on Black (Borders, frame separators, rainbow stage 3)
  - `Pair 6`: Magenta on Black (Rainbow stage 4)
  - `Pair 7`: Blue on Black (Rainbow stage 5)
  - `Pair 8`: White on Black (Standard un-typed text)
- **Rainbow FX Algorithm**: Correctly typed characters cycle through `rainbow_pairs[(i + rainbow_tick) % 5]` with `A_BOLD`. Mismatched characters immediately render with `Pair 3` (`COLOR_RED`).
- **Console Layout**: Implements vertical split layout (`render_console_sidebar`) using `ACS_VLINE` alongside a horizontal bottom input bar (`render_input_bar`) with `ACS_HLINE`.

### 2.5 `words.c` / `words.h`
- Manages three independent dynamically allocated `WordBank` instances (`g_words`, `g_congrats`, `g_loser`).
- Resolves file locations across multiple relative candidate paths:
  1. `./<filename>`
  2. `build/Debug/<filename>`
  3. `../<filename>`
  4. `../../<filename>`
  5. Directory of the executable binary via `GetModuleFileNameA` (Windows).
- Includes embedded fallback arrays to guarantee non-null operation in missing-file scenarios.

### 2.6 `score.c` / `score.h`
- Encapsulates cross-platform score loading and synchronization.
- **Windows Implementation (`_WIN32`)**:
  - Uses Win32 Registry API (`advapi32.lib`).
  - Key: `HKEY_CURRENT_USER\Software\TypeAttack`
  - Value: `TotalScore` (`REG_DWORD`).
- **POSIX / Linux Implementation**:
  - Employs local obfuscated storage (`.score.dat`).
  - Score value is masked using bitwise XOR and additive constant:
    $$\text{Obfuscated} = (\text{Score} \oplus \text{0xA5C3F109}) + \text{0x1337BEEF}$$
  - Computes SHA-256 hash (FIPS 180-2 compliant) over `SALT + ScoreString`.
  - On load, recalculates hash and validates authenticity before returning the decrypted score value.

### 2.7 `resource.h` / `resource.rc`
- Defines Windows application metadata and executable icon resource (`IDI_APP_ICON` with ID `101`).
- Compliant with `VS_VERSION_INFO` specification for Windows PE binaries.

---

## 3. Compilation Dependencies and Linkage

- **Windows**:
  - C Compiler: MSVC (`cl.exe`) or GCC (MinGW).
  - Include Path: `includes/` (contains `curses.h`).
  - Library Linkage: `pdcurses.lib` (or `pdcurses.a`), `advapi32.lib`.
- **Linux**:
  - C Compiler: GCC or Clang.
  - Library Linkage: `libncurses` / `libcurses`.
