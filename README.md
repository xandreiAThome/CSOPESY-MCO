# CSOPESY MCO

## Requirements

- CMake 3.21 or newer
- A C++20-capable compiler
- Linux/macOS: GNU Make or another CMake-compatible build tool
- Windows: Visual Studio 2022 or Visual Studio Build Tools

## Project Layout

- `src/` - implementation files
- `include/` - public headers
- `build/` - local CMake build output
- `openspec/` - OpenSpec change files and specs

## Build

### Linux/macOS

Configure and build with the Linux presets:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
```

For a release build, use `linux-release` instead of `linux-debug`.

### Windows

Use the Windows presets with Visual Studio 2022:

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug
```

For a release build, use `windows-release` instead of `windows-debug`.

## Visual Studio IDE Setup

To use the project in Visual Studio 2026:

1. Open the project folder in Visual Studio.
2. Let Visual Studio detect the CMake project automatically.
3. Select the CMake preset you want to use, such as `windows-debug` or `windows-release`.
4. Build and run from Visual Studio using the standard CMake controls.

If Visual Studio asks for a kit or configuration, choose the Windows preset that matches the build type you want.

## Visual Studio Code

Open the folder in VS Code and use the CMake Tools extension.

- On Linux/macOS, choose `linux-debug` or `linux-release`.
- On Windows, choose `windows-debug` or `windows-release`.

The project uses CMake presets, so VS Code can configure the same build layout consistently across machines.

## Run

After building, run the executable from the build directory:

```bash
./build/linux-debug/csopesy
```

On Windows, the executable is located in the matching preset build folder.
