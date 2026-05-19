## 1. Foundation

- [ ] 1.1 Create the C++ application entry point and basic project layout
- [ ] 1.2 Implement configuration loading from `config.txt`
- [ ] 1.3 Add initialization state so commands are blocked until `initialize` succeeds

## 2. Core Process Model

- [ ] 2.1 Define the process state structure for ids, names, instructions, variables, logs, and lifecycle flags
- [ ] 2.2 Implement instruction parsing into an internal representation
- [ ] 2.3 Add execution logic for `PRINT`, `DECLARE`, `ADD`, `SUBTRACT`, `SLEEP`, and nested `FOR`
- [ ] 2.4 Add clamping and auto-declare behavior for variables

## 3. Scheduler and Screens

- [ ] 3.1 Implement the real-time scheduler loop and tick counter
- [ ] 3.2 Add process generation for `scheduler -start` and stop control for `scheduler -stop`
- [ ] 3.3 Implement `screen -s` and `screen -r` navigation between main menu and process screens
- [ ] 3.4 Implement `process -smi` to print live process information and logs

## 4. Reporting and Verification

- [ ] 4.1 Implement shared utilization summary formatting for `screen -ls` and `report -util`
- [ ] 4.2 Write `report -util` output to `csopesy-log.txt`
- [ ] 4.3 Validate command sequences against the spec and tune output text for exact matches