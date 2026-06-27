# CSOPESY MCO — Process Scheduler Simulator

## Author(s)
- ESPONILLA ELLEXANDREI ALCERA
- INTINO RADNY ASIS
- MANGUBAT MARVIN IVAN CASTROVERDE
- REGALA JOSE CARLOS ARRIOLA

---

## Entry Point & Main Function
* **Main Function Location:** `src/main.cpp`
* **Entry Manager Class:** `Console` ([include/console.hpp](include/console.hpp) / [src/console.cpp](src/console.cpp))

---

## Requirements

- CMake 3.21 or newer
- A C++20-capable compiler
- Linux/macOS: GNU Make or another CMake-compatible build tool
- Windows: Visual Studio 2022 / 2026 or Visual Studio Build Tools

---

## Project Layout

- `src/` - Implementation files (`main.cpp`, `console.cpp`, schedulers, commands)
- `include/` - Public header files
- `build/` - Local CMake build output
- `assets/` - Simulator assets and text templates
- `openspec/` - OpenSpec specification and change tracking files

---

## Build Instructions

### Linux / macOS

Configure and build with the debug preset:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
```

For a release build, substitute `linux-debug` with `linux-release`.

### Windows

Use the Windows presets with Visual Studio or PowerShell:

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug
```

For a release build, substitute `windows-debug` with `windows-release`.

---

## Running the Simulator

> **IMPORTANT:** The executable **must be run from the repository root directory** so it can find `config.txt` and `assets/introText.txt`.

### Execution Command:

```bash
./build/linux-debug/csopesy
```

*(On Windows, run `.\build\windows-debug\csopesy.exe`)*

---

## Simulator CLI Commands

| Command | Allowed States | Description |
| :--- | :--- | :--- |
| `initialize` | Pre-init | Loads `config.txt` and initializes the process scheduler. |
| `scheduler-test` | Main Menu | Starts periodic batch process generation. |
| `scheduler-stop` | Main Menu | Stops periodic batch process generation. |
| `screen -ls` | Main Menu | Displays CPU status, active cores, and process lists. |
| `screen -s <name>` | Main Menu | Creates a new process and enters its dedicated screen. |
| `screen -r <name>` | Main Menu | Resumes an existing process screen. |
| `process-smi` | Process Screen | Displays process logs, state, and execution progress. |
| `report-util` | Main Menu | Saves the full process utilization report to `csopesy-log.txt`. |
| `clear` | Any | Clears the terminal screen. |
| `exit` | Any | Exits process screen (back to menu) or shuts down simulator. |
