## Why

The CSOPESY major output requires a console emulator that behaves like a real CLI plus a live CPU scheduler. Building it as a structured OpenSpec change gives a clear contract for the command flow, process lifecycle, and reporting behavior before implementation begins.

## What Changes

- Add a C++ command-line application with a main menu, screen mode, and process reporting flow.
- Add initialization from `config.txt` and enforce `initialize` as the first accepted command.
- Add a live CPU scheduler that creates and runs processes in real time while the console stays active.
- Add process instruction execution for `PRINT`, `DECLARE`, `ADD`, `SUBTRACT`, `SLEEP`, and nested `FOR` blocks.
- Add `screen -s`, `screen -r`, `screen -ls`, `process -smi`, `scheduler -start`, `scheduler -stop`, and `report -util`.
- Add persisted utilization output to `csopesy-log.txt`.

## Capabilities

### New Capabilities
- `csopesy-process-scheduler-cli`: interactive CLI emulator, screen multiplexer behavior, scheduler, process execution, and utilization reporting.

### Modified Capabilities
- None.

## Impact

Affected areas include the C++ entry point, command parser, scheduler runtime, process model, instruction interpreter, process registry, configuration loader, and report generator. The change also introduces file output for utilization logs and concurrent state management between the console thread and scheduler loop.