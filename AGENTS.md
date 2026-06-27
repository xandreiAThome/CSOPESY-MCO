# CSOPESY MCO — Agent Instructions

## Build & Run

```bash
cmake --preset linux-debug && cmake --build --preset linux-debug
./build/linux-debug/csopesy
```

No tests, no linters, no typecheck, no CI. Only build verification matters.
Presets: `linux-debug`, `linux-release`, `windows-debug`, `windows-release`.
Compiler flags: `-Wall -Wextra -pedantic`, C++20.

## Runtime Must-Knows

- Executable **must run from repo root** — `config.txt`, `assets/introText.txt`, and `csopesy-log.txt` are relative to CWD.
- `build/` is gitignored. Assets are copied from `assets/` to output dir at build time.
- `.clangd` expects compilation DB at `build/linux-debug`; regenerate if you change presets.

## Architecture

Single executable, no packages/libraries. Key modules:

| Module | Location | Role |
|---|---|---|
| Console | `src/console.cpp` / `include/console.hpp` | CLI main loop, state machine |
| Console states | `src/console_state.cpp` / `include/console_state.hpp` | `Uninitialized` → `MainMenu` ↔ `ProcessScreen` |
| Parser | `src/parser.cpp` / `include/parser.hpp` | Tokenizes input into `ParsedCommand` |
| Globals | `src/globals.cpp` / `include/globals.hpp` | Singleton — config + `cpuCycles` tick counter |
| GlobalScheduler | `src/scheduler/global_scheduler.cpp` / `include/scheduler/global_scheduler.hpp` | Singleton — manages cores, workers, queues, lifecycle |
| FCFSScheduler | `src/scheduler/fcfsscheduler.cpp` / `include/scheduler/fcfsscheduler.hpp` | Per-core queue, implements `IScheduler` |
| Process | `include/scheduler/process.hpp` | State machine: READY/WAITING/RUNNING/FINISHED, variable map, logs |
| ProcessFactory | `src/scheduler/process_factory.cpp` / `include/scheduler/process_factory.hpp` | Generates random commands with nested FOR loops (depth ≤ 3) |
| Commands | `src/commands/*`, `include/commands/*` | `ICommand` impls: Print, Declare, Add, Subtract, Sleep |

## CLI Commands

Only accepted in specific console states:

| Command | States | Notes |
|---|---|---|
| `initialize` | Uninitialized | Loads `config.txt`; must succeed to proceed |
| `exit` | Any | Shuts down scheduler threads, exits main menu, back from process screen |
| `clear` | Any | Clears terminal |
| `screen -ls` | MainMenu | Lists all processes — running + finished |
| `screen -s <name>` | MainMenu | Creates process, enters its screen |
| `screen -r <name>` | MainMenu | Resumes existing process's screen |
| `process-smi` | ProcessScreen | Shows process logs, state, progress |
| `scheduler-start` | MainMenu | Starts periodic batch process generation |
| `scheduler-stop` | MainMenu | Stops batch process generation |
| `report-util` | MainMenu | Writes full process report to `csopesy-log.txt` |

## OpenSpec Workflow

This repo uses OpenSpec for change management. Skills are registered in both `.opencode/skills/` and `.github/skills/`. The active change lives in `openspec/changes/csopesy-process-scheduler-cli/`. Use the provided OpenSpec skills (`openspec-*`) for the structured workflow — all 11 steps (explore → propose → apply → verify → archive) are available as skills.

## Config (`config.txt`)

```
num-cpu 128
scheduler "fcfs"        # or "rr" (only FCFS implemented)
quantum-cycles 5
batch-process-freq 10
min-ins 100
max-ins 100
delay-per-exec 2
```

## Notable Quirks

- `std::rand()` is used without seeding — process instruction sequences are deterministic across runs.
- Tick thread runs at ~60fps (16ms sleep). All scheduler timing derives from `Globals::cpuCycles`.
- `IETThread::sleep()` takes milliseconds, not ticks. Core workers busy-wait on `cpuCycles` for tick-based delays.
- Default VS Code launch configs target individual file compilation (not the CMake target) — use CMake Tools extension or CLI instead.
