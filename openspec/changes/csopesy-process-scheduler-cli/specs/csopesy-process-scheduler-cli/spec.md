## ADDED Requirements

### Requirement: Initialization gate
The system MUST require `initialize` to succeed before recognizing any command other than `exit`.

#### Scenario: Reject commands before initialization
- **WHEN** the user enters a command before `initialize`
- **THEN** the system MUST ignore the command and keep the console in the pre-initialized state

#### Scenario: Allow exit before initialization
- **WHEN** the user enters `exit` before `initialize`
- **THEN** the system MUST terminate the console

### Requirement: Configuration loading
The system MUST read scheduler and process settings from `config.txt` when `initialize` is executed.

#### Scenario: Load configuration values
- **WHEN** the user runs `initialize`
- **THEN** the system MUST parse the configured CPU count, scheduler algorithm, quantum, process frequency, instruction bounds, and execution delay

#### Scenario: Reject invalid configuration
- **WHEN** `config.txt` contains an invalid value for a required parameter
- **THEN** the system MUST not enter the initialized state

### Requirement: Main menu command support
The system MUST recognize `screen`, `scheduler -start`, `scheduler -stop`, `report -util`, and `exit` from the main menu after initialization.

#### Scenario: Open a process screen
- **WHEN** the user enters `screen -s <process name>` from the main menu
- **THEN** the system MUST open the named process screen if the name is available

#### Scenario: List screens
- **WHEN** the user enters `screen -ls`
- **THEN** the system MUST display the current utilization summary and process list

#### Scenario: Start and stop dummy process generation
- **WHEN** the user enters `scheduler -start` and later `scheduler -stop`
- **THEN** the system MUST begin and then stop generating dummy processes according to the configured frequency

### Requirement: Process screen behavior
The system MUST support `process -smi` and `exit` while inside a process screen.

#### Scenario: Show process information
- **WHEN** the user enters `process -smi`
- **THEN** the system MUST display the process identity, current state, and accumulated print logs

#### Scenario: Return to main menu
- **WHEN** the user enters `exit` from a process screen
- **THEN** the system MUST return the user to the main menu

### Requirement: Process lifecycle visibility
The system MUST allow a process screen to be reopened with `screen -r <process name>` while the process is still known to the system.

#### Scenario: Resume an existing process screen
- **WHEN** the user enters `screen -r <process name>` for a known process
- **THEN** the system MUST reopen that process screen

#### Scenario: Report missing process
- **WHEN** the user enters `screen -r <process name>` for an unknown or finished process
- **THEN** the system MUST display `Process <process name> not found.`

### Requirement: Instruction execution
The system MUST execute process instructions for `PRINT`, `DECLARE`, `ADD`, `SUBTRACT`, `SLEEP`, and nested `FOR` blocks.

#### Scenario: Execute print and arithmetic instructions
- **WHEN** a process executes `PRINT`, `DECLARE`, `ADD`, or `SUBTRACT`
- **THEN** the system MUST update process state and preserve the resulting variables and log output

#### Scenario: Suspend on sleep
- **WHEN** a process executes `SLEEP(X)`
- **THEN** the system MUST relinquish the CPU for the configured number of ticks

#### Scenario: Support nested loops
- **WHEN** a process executes nested `FOR` blocks up to three levels deep
- **THEN** the system MUST execute the nested instruction sequence correctly

### Requirement: Reporting
The system MUST generate the same utilization summary for `screen -ls` and `report -util`, and `report -util` MUST save the summary to `csopesy-log.txt`.

#### Scenario: Display utilization summary
- **WHEN** the user enters `screen -ls`
- **THEN** the system MUST show CPU utilization, used cores, available cores, and a summary of running and finished processes

#### Scenario: Persist utilization summary
- **WHEN** the user enters `report -util`
- **THEN** the system MUST write the same summary to `csopesy-log.txt`
