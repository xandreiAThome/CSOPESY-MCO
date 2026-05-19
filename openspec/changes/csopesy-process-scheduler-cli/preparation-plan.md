# CSOPESY Process Scheduler CLI — Preparation Plan

## Reading Resources

| Topic | Resource | Why |
|---|---|---|
| C++ concurrency | [*C++ Concurrency in Action*](https://www.manning.com/books/c-plus-plus-concurrency-in-action-second-edition) by Anthony Williams (ch 2-4) | Threads, mutexes, lock design. Chapter 4 has a thread-safe queue pattern that maps to the process registry. |
| C++ STL | [cppreference.com](https://cppreference.com) — `vector`, `map`, `queue`, `string`, `stringstream` | Reference. Look things up as needed. |
| CPU scheduling | [*Operating Systems: Three Easy Pieces* (OSTEP)](https://pages.cs.wisc.edu/~remzi/OSTEP/) — Chapters 4-7 | Clearest explanation of FCFS, RR, context switching, scheduling metrics. Free online. |
| State machines | [*Practical Statecharts in C/C++*](https://www.state-machine.com/psicc) by Miro Samek (ch 1-2) | Application mode machine pattern. Free PDF available. |
| Parsing | [*Crafting Interpreters*](https://craftinginterpreters.com) by Robert Nystrom (ch 1-4) | Tokenization and AST. Free online. Maps to instruction parser. |
| Console ANSI codes | [ANSI escape code reference](https://en.wikipedia.org/wiki/ANSI_escape_code) + [cheatsheet](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797) | ~3 codes needed: clear screen, cursor home, reset. |

---

## Exercises (Warmup → Integration)

### Exercise 1: Thread-safe counter

**Goal**: Get comfortable with `std::thread` and `std::mutex`.

Write a program that:
- Spawns 3 threads
- Each thread increments a shared counter 1,000,000 times
- Main thread reads the final value
- Add a second counter that each thread decrements after N iterations of incrementing

**Variation**: Make the main thread pause/resume workers using an atomic flag.

**Why**: Scheduler thread shares process registry with console thread. Same pattern — shared state behind mutex, background loop reading/writing, foreground reading.

**Learn from it**: `lock_guard`, critical section pattern, why `atomic<bool>` for stop flags, clean thread joining.

---

### Exercise 2: Enum-based state machine

**Goal**: Implement the application mode transitions.

Implement a struct `StateMachine` with:
- An enum `{ UNINIT, MAIN_MENU, PROCESS_SCREEN }`
- A transition table: which transitions are legal
- A `handleCommand(string)` function that returns an error if transition isn't allowed

Transitions:
```
UNINIT → "initialize" → MAIN_MENU
MAIN_MENU → "screen -s <name>" → PROCESS_SCREEN
PROCESS_SCREEN → "exit" → MAIN_MENU
UNINIT → any other command → stays UNINIT
```

**Variation**: Add a `currentScreen` field storing the process name when entering PROCESS_SCREEN. Clearing it on exit.

---

### Exercise 3: Tick-based scheduler simulator

**Goal**: Understand the CPU tick loop. Single-threaded first.

Implement:
- `struct Process` with name, totalInstructions, executedInstructions, state
- `struct Scheduler` with numCores, readyQueue, core array, currentTick

Tick loop:
1. `tick++`
2. For each core: if process finished, mark done
3. For each idle core: pop from readyQueue and assign
4. For each running process: `executeOne()` → `executedInstructions++`

**Variation**: Add Round Robin with quantum. When process runs for `quantum` ticks on a core, move it to back of ready queue.

---

### Exercise 4: Instruction parser (mini interpreter)

**Goal**: Parse and execute simple instructions.

Define `struct Instruction` with:
- `enum OpCode { PRINT, DECLARE, ADD, SUBTRACT, SLEEP }`
- `vector<string> args`

Parser converts raw strings (`"DECLARE(x, 10)"`, `"ADD(x, x, 5)"`) into `vector<Instruction>`.

Executor has:
- `map<string, uint16_t>` for variables
- `vector<string>` logBuffer
- `pc` (instruction index)
- `Step()`: executes one instruction, advances pc, returns bool (still running)

Clamping: values wrap at `[0, 65535]`.
Auto-declare: undeclared variables default to 0.

**Variation (harder)**: Implement `FOR([instructions], repeats)` with a loop-frame stack:
```cpp
struct LoopFrame {
    int bodyStartPC;
    int returnPC;
    int iterationsLeft;
};
```

---

### Exercise 5: Console screen switching

**Goal**: Simulate main menu / process screen split.

Write a minimal program:
- Prints `"MAIN MENU > "` and waits
- On `"screen -s <name>"`: shows `"[entering screen <name>]"`, loops with `"SCREEN <name> > "` prompt
  - On `"exit"`: back to main menu
  - On `"process -smi"`: prints simulated process info
- On `"exit"` in main menu: quits

Focus on prompt switching and two input loops. Use `getline(cin, line)` and trim whitespace.

---

### Exercise 6: Single-threaded prototype

**Goal**: Combine Exercises 3 + 4.

- Create N random processes with random instructions
- Scheduler ticks through them sequentially
- After each tick: type `"status"` to see process state or `"tick"` to advance
- Print formatted: `"Process: p01 | ID: 1 | Instructions: 5/12"`

---

### Exercise 7: Background scheduler thread

**Goal**: Move tick loop to background thread.

- Scheduler thread: ticks every 500ms
- Console thread: reads commands, can type `"ps"` to list, `"stop"` to halt
- Shared: mutex-guarded `vector<Process>`
- Clean shutdown: set flag, join thread

---

### Exercise 8: Full integration

Combine everything:
- State machine (Ex 2) + Scheduler thread (Ex 7) + Instruction executor (Ex 4) + Console switching (Ex 5)
- Add `config.txt` loading
- Report formatting
- Test scenario:
  1. `initialize`
  2. `scheduler -start`
  3. `screen -s p01`
  4. `process -smi` (see instructions executing)
  5. `exit`
  6. `screen -ls`
  7. `scheduler -stop`
  8. `report -util`
  9. `exit`

---

## Suggested Schedule

| Week | Focus | Exercise |
|---|---|---|
| 1 | C++ concurrency + OSTEP Ch 4-7 | 1 (thread-safe counter) |
| 2 | State machines + string parsing | 2 (state machine) + 5 (console switching) |
| 3 | Scheduling algorithms + tick loop | 3 (tick simulator) |
| 4 | Instruction parsing and execution | 4 (parser + executor) |
| 5 | Integration — single-threaded | 6 (prototype) |
| 6 | Threading integration | 7 (background thread) |
| 7 | Full integration | 8 |
| 8 | Polish + quiz prep | Exact output matching |
