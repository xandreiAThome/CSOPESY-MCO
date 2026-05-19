## Context

This change implements the CSOPESY process scheduler and CLI as a C++ console application. The spec requires a pre-initialization gate, a main menu, a process screen that behaves like a screen multiplexer, a live scheduler, process instruction interpretation, and utilization reporting. The design must work with real-time background execution while the user types commands.

## Goals / Non-Goals

**Goals:**
- Provide a deterministic command model for the quiz system.
- Keep process state, scheduler state, and console state consistent under concurrency.
- Map the spec’s process instructions to a small in-memory execution model.
- Support both interactive display and file-backed utilization reporting.

**Non-Goals:**
- Emulating a full operating system shell.
- Persisting state across application restarts.
- Implementing a general-purpose scripting language.

## Decisions

- Use a shared application core with two visible modes: main menu and process screen. This keeps the CLI behavior simple while matching the spec’s screen transitions.
- Run the scheduler on a background thread and keep the console on the foreground thread. A shared process table guarded by a mutex is the safest fit for C++ because process state changes while commands are read.
- Model each process as a small state machine with fields for name, id, instruction list, program counter, variables, log buffer, sleep countdown, and finish status. This keeps `process -smi`, `screen -ls`, and `report -util` reading the same source of truth.
- Parse instructions into an intermediate representation instead of interpreting raw strings repeatedly. That makes nested `FOR` blocks and repeated scheduling passes easier to handle.
- Treat CPU ticks as scheduler iterations rather than wall-clock time. This keeps the implementation aligned with the spec’s framing and makes behavior easier to test.
- Use a single reporting formatter for both `screen -ls` and `report -util`. The only difference should be the sink: console versus file.
- Prefer standard C++ facilities (`std::thread`, `std::mutex`, `std::condition_variable`, `std::filesystem`, `std::chrono`) to avoid extra dependencies.

## Risks / Trade-offs

- [Concurrency bugs] → Keep all process registry mutations behind one lock and separate command parsing from state mutation.
- [Tick drift or nondeterminism] → Drive scheduling from a tick counter owned by the scheduler loop instead of ad hoc sleeps.
- [Nested loop complexity] → Limit loop depth explicitly to three and validate it during parsing.
- [Report inconsistency] → Format utilization output from a single shared function so the console and log file cannot diverge.
- [Quiz-case sensitivity] → Keep command parsing strict but predictable, and preserve exact error text where the spec defines it.

## Migration Plan

1. Implement configuration loading and the initialization gate.
2. Add the process model, instruction representation, and parser.
3. Implement the scheduler loop, process creation, and CPU tick accounting.
4. Add screen mode navigation and process status commands.
5. Add utilization reporting for console and file output.
6. Validate against sample command sequences and adjust formatting only where the spec requires it.

## Open Questions

- How strict should command tokenization be for extra spaces and casing differences?
- Should a process created by `screen -s` begin execution immediately or wait for scheduler placement?
- What exact fields must appear in the utilization summary beyond the spec’s minimum items?
- How should finished process logs be retained when a process is reopened through `screen -r`?