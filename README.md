# DoodleJump

A lightweight implementation of the classic DoodleJump game, written in C++ using Windows GDI+. The game features platform jumping mechanics, shooting mechanics, enemies, and score tracking.

https://github.com/user-attachments/assets/2a3dc1c3-dedd-47e1-a13d-5973646328b2


## Features
- Platform-independent core logic
- **Custom framework**
- Command line arguments support
- Lightweight build (< 250kb)

## Prerequisites

To build and run the app, you need:
- GDI+ library (included in Windows)
- Visual Studio 2019/2022 with C++ support or CMake 3.10+
- Windows SDK with GDI+ development files

## Custom Framework
The game is built on a lightweight custom framework that provides:
- Double-buffered GDI+ rendering system
- Sprite management with rotation support
- Window management and input handling
- Resource loading and memory management
- Event-driven architecture with callbacks for inputs and loop timing:
- Command line argument parsing for window configuration
- A simple C-style API

## Building the Project using CMake

```bash
# Clone the repository
git clone https://github.com/your-username/DoodleJump.git
cd DoodleJump

# Option 1: Using build script
./build.bat

# Option 2: Manual CMake build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

The executable and required resources will be placed in the `build` directory.

## Running the Game

### Basic Launch
```bash
DoodleJump.exe
```

### Command Line Arguments
The game supports the following command line arguments:
```bash
DoodleJump.exe [width] [height] [fullscreen]
```
- `width`: Window width in pixels (default: 800)
- `height`: Window height in pixels (default: 600)
- `fullscreen`: 0 for windowed mode, 1 for fullscreen (default: 0)

Example:
```bash
DoodleJump.exe 1024 768 1  # Launches in fullscreen at 1024x768
```
