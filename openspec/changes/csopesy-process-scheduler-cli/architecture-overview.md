# CSOPESY Process Scheduler CLI — Architecture Overview

## The Five Architectural Layers

```
┌─────────────────────────────────────────────────────┐
│                   APPLICATION CORE                  │
│  ┌──────────┐  ┌──────────┐  ┌───────────────────┐  │
│  │  Config   │  │  State   │  │  Command Router  │  │
│  │  Loader   │  │ Machine  │  │  (dispatcher)    │  │
│  └──────────┘  └──────────┘  └───────────────────┘  │
├─────────────────────────────────────────────────────┤
│                    SCHEDULER                        │
│  ┌──────────────┐  ┌────────────┐  ┌──────────────┐ │
│  │  Tick Loop   │  │ Ready Queue│  │  Algorithm   │ │
│  │  (bg thread) │  │            │  │  (FCFS / RR) │ │
│  └──────────────┘  └────────────┘  └──────────────┘ │
├─────────────────────────────────────────────────────┤
│                    PROCESS MODEL                    │
│  ┌────────────────┐  ┌────────────────────────────┐ │
│  │  Process State  │  │  Instruction Executor     │ │
│  │  (PCB struct)   │  │  (tiny VM)                │ │
│  └────────────────┘  └────────────────────────────┘ │
├─────────────────────────────────────────────────────┤
│                    CLI LAYER                        │
│  ┌──────────┐  ┌──────────┐  ┌───────────────────┐  │
│  │  Main    │  │  Process │  │  Report Formatter │  │
│  │  Menu    │  │  Screen  │  │  (ls / report-    │  │
│  │          │  │  Mode    │  │   util)           │  │
│  └──────────┘  └──────────┘  └───────────────────┘  │
├─────────────────────────────────────────────────────┤
│                    SHARED STATE                     │
│  ┌─────────────────────────────────────────────────┐│
│  │       Process Registry (guarded by mutex)       ││
│  │    ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐          ││
│  │    │ p1   │ │ p2   │ │ p3   │ │ ...  │          ││
│  │    └──────┘ └──────┘ └──────┘ └──────┘          ││
│  └─────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────┘
```

---

## 1. Application State Machine

The program has distinct modes with strict transitions:

```
                    ┌──────────┐
                    │ UNINIT   │
                    │ (pre-init)│
                    └────┬─────┘
                         │ "initialize" (reads config.txt)
                         │
                    ┌────▼─────┐
                    │  MAIN    │◄──────────────────────┐
                    │  MENU    │                        │
                    └────┬─────┘                        │
                         │                              │
                    ┌────▼─────┐                        │
                    │ PROCESS  │── "exit" ──────────────┘
                    │  SCREEN  │
                    └──────────┘
                         │
                         ▼
                    ┌──────────┐
                    │ FINISHED │ (process died)
                    │ (can't re-│
                    │  attach)  │
                    └──────────┘
```

- Mode tracked as an enum `{ UNINIT, MAIN_MENU, PROCESS_SCREEN }`
- PROCESS_SCREEN mode also stores a pointer/reference to the attached process
- Commands are routed based on current mode

---

## 2. Scheduler (Background Thread)

The most complex component. Runs on a background thread while the console stays on the foreground.

```
Scheduler Thread                    Console Thread (main)
┌─────────────────────┐            ┌──────────────────────┐
│  while(running) {   │            │  while(true) {       │
│    tick++;          │            │    read command;     │
│    if(tick % freq   │            │    route command;    │
│       == 0) {       │            │  }                   │
│      createProcess()│            └──────────────────────┘
│    }                │
│                     │            ┌──────────────────────┐
│    for(each core) { │            │   SHARED:           │
│      if(core free)  │            │   ProcessRegistry   │
│        dispatch()   │            │   (mutex-guarded)   │
│    }                │            └──────────────────────┘
│                     │
│    for(each running │
│     process) {      │
│      executeOneIns()│
│      if(delay>0)    │
│        skip ticks   │
│    }                │
│                     │
│    for(each sleep)  │
│      decrement();   │
│                     │
│    sleep(1 tick)    │
│  }                  │
└─────────────────────┘
```

### Key Design Questions

- **What IS a "CPU tick"?** Integer counter. Each tick needs a `sleep/delay` to avoid pegging CPU at 100%. Duration affects how fast processes execute relative to user interaction.
- **Core management**: Up to 128 cores. Each core runs one process at a time.
- **FCFS vs RR**: FCFS runs to completion. RR preempts after `quantum-cycles` ticks.
- **delay-per-exec**: Process stays on core but skips instruction advancement for X ticks.

---

## 3. Process Model — Tiny Virtual Machine

Each process is a miniature computer:

```
┌──────────────────────────────────────────────┐
│              PROCESS CONTROL BLOCK            │
├──────────────────────────────────────────────┤
│  pid: uint                                    │
│  name: string (e.g. "p01", "hello")          │
│  state: READY | RUNNING | FINISHED           │
│  pc: uint (instruction index)                 │
│  instructions: Instruction[]                  │
│  variables: map<string, uint16>              │
│  logBuffer: string[]                          │
│  sleepCountdown: uint                         │
│  delayCountdown: uint                         │
│  coreAssigned: int (-1 if not running)        │
└──────────────────────────────────────────────┘
```

### Instruction Representation

```cpp
enum class OpCode { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR, END_FOR };

struct Instruction {
    OpCode op;
    vector<string> args;        // e.g., for ADD: {"x", "y", "z"}
    vector<Instruction> body;   // for FOR blocks (nested!)
};
```

### Nested FOR Loop Execution

FOR loops require a stack of loop frames (up to 3 levels deep):

```
┌──────────────────────┐
│  LOOP FRAME          │
├──────────────────────┤
│  instructionIndex: 0 │  ← position within body
│  iterationsLeft: 5   │  ← repeats remaining
│  bodyStartPC: 12     │  ← where FOR starts in main list
│  returnPC: 17        │  ← where to continue after FOR ends
└──────────────────────┘
```

The PC alone is insufficient — you need this call-stack approach.

### Variable Rules

- `uint16_t` clamped to `[0, 65535]`
- Auto-declare: undeclared variables default to 0 when used in ADD/SUBTRACT

---

## 4. CLI / Screen Multiplexer

### Screen Navigation Flow

```
┌────────────────────────────────┐
│  MAIN MENU MODE                │
│                                │
│  > screen -s myProcess         │
│  [console clears]              │
│  ─────────────────────────     │
│  PROCESS SCREEN: myProcess     │
│  >                             │
│  > process -smi                │
│  Process: myProcess            │
│  ID: 3                         │
│  Log:                          │
│    Hello world from myProcess! │
│  > exit                        │
│  [back to main menu]           │
│                                │
│  > screen -ls                  │
│  CPU Utilization: 45%          │
│  Cores Used: 2/4               │
│  Running: p01, p02             │
│  Finished: myProcess           │
│  > report -util                │
│  [saved to csopesy-log.txt]    │
└────────────────────────────────┘
```

### Report Formatter

Shared formatting for both `screen -ls` (console) and `report -util` (file):

```
                  ┌──────────────────┐
                  │  ReportGenerator │
                  │  ::formatSummary │
                  └────────┬─────────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
         ┌────▼────┐ ┌────▼────┐ ┌────▼────┐
         │  screen │  │  report │  │ screen  │
         │  -ls    │  │  -util  │  │  -smi   │
         │(console)│  │ (file)  │  │(console)│
         └─────────┘  └─────────┘  └─────────┘
```

---

## 5. Command Parsing Strategy

Tokenize user input and match against known commands:

```
Input: "screen -s myProcess"
        │      │   │
        │      │   └── arg1: process name
        │      └────── flag: -s (create) / -r (resume) / -ls (list)
        └────────────── verb: screen

Input: "scheduler -start"
        │          │
        │          └── flag
        └────────────── verb
```

---

## Thread Architecture

```
┌──────────────────────────────────────────────────┐
│                    MAIN THREAD                     │
│  ┌──────────────────────────────────────────────┐ │
│  │  Console I/O Loop                             │ │
│  │  - reads user input                          │ │
│  │  - routes commands                           │ │
│  │  - switches mode                             │ │
│  └──────────────────────────────────────────────┘ │
│                         │                          │
│                         │ (shared via mutex)       │
│                         ▼                          │
│  ┌──────────────────────────────────────────────┐ │
│  │         PROCESS REGISTRY                      │ │
│  │  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐            │ │
│  │  │ p1  │ │ p2  │ │ p3  │ │ p4  │ ...        │ │
│  │  └─────┘ └─────┘ └─────┘ └─────┘            │ │
│  │         ▲                                     │ │
│  └─────────┼─────────────────────────────────────┘ │
│            │                                       │
│  ┌─────────┴─────────────────────────────────────┐ │
│  │           SCHEDULER THREAD                     │ │
│  │  while(running) {                              │ │
│  │    lock(mutex);                                │ │
│  │    create processes if due;                    │ │
│  │    dispatch to free cores;                     │ │
│  │    for each running process:                   │ │
│  │       execute next instruction;                │ │
│  │       handle SLEEP/delay;                      │ │
│  │       mark finished if done;                   │ │
│  │    unlock(mutex);                              │ │
│  │    sleep(tick_duration);                       │ │
│  │  }                                             │ │
│  └────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────┘
```

---

## Hardest Parts (Complexity Ranking)

1. **Scheduler concurrency** — background thread + console thread sharing process registry. A process might finish between `screen -r` and entering its screen.

2. **Instruction execution with delays** — SLEEP and delay-per-exec must suspend instruction advancement without blocking the whole scheduler.

3. **Nested FOR loops** — need a stack of loop frames (up to 3 deep). PC becomes a tree position, not linear index.

4. **Exact output format** — quiz grading checks exact strings. Must match sample output precisely.

---

## Open Design Questions

- Should a process created by `screen -s` begin execution immediately or wait for scheduler placement?
- How should finished process logs be retained when reopened through `screen -r`?
- How strict should command tokenization be (casing, extra spaces)?
- What exact fields appear in utilization summary?
- How long is one CPU tick in real time?
