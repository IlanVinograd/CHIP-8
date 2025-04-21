# CHIP-8 Emulator

A CHIP-8 emulator developed in modern C++ using WinAPI and GDI+. This project is designed for Windows systems and features a responsive graphical interface, interactive controls, and built-in debugging tools. Built from the ground up to serve both as a learning platform and a technical showcase of low-level programming.

## Game Previews

The following GIFs demonstrate a few classic games running on this emulator:

**RushHour:**
![RushHour](https://github.com/IlanVinograd/CHIP-8/blob/main/Img/Game1.gif)

**Space Invaders:**
![Space Invaders](https://github.com/IlanVinograd/CHIP-8/blob/main/Img/Game2.gif)

**Brix:**
![Brix](https://github.com/IlanVinograd/CHIP-8/blob/main/Img/Game3.gif)

**Tetris:**
![Tetris](https://github.com/IlanVinograd/CHIP-8/blob/main/Img/Game4.gif)

## Keyboard Layout

The emulator maps your keyboard to the original CHIP-8 hexadecimal keypad as follows:

**CHIP-8 Keypad Layout:**

| 1 | 2 | 3 | C |
|---|---|---|---|
| 4 | 5 | 6 | D |
| 7 | 8 | 9 | E |
| A | 0 | B | F |

**Keyboard Mapping:**

| 1 | 2 | 3 | 4 |
|---|---|---|---|
| Q | W | E | R |
| A | S | D | F |
| Z | X | C | V |




## How to Run

### Requirements

- **Windows OS only** – This emulator uses native WinAPI and GDI+ for rendering and user interface.
- **No additional dependencies** – A C++17-compatible compiler and `make` utility are required (e.g., MinGW or MSYS2).

> This emulator is not compatible with macOS or Linux due to its use of Windows-specific APIs.

### Running a Game

To run a CHIP-8 ROM, click the **Load** button to select a `.ch8` file. Once loaded, click **Start** to begin execution:
![Choose Game](https://github.com/IlanVinograd/CHIP-8/blob/main/Img/CG.gif)

### Build Instructions

1. Open a terminal and navigate to the `src` directory:

   ```bash
   cd src
   ```

2. Build and launch the emulator:

   ```bash
   make
   ```

3. To clean up generated files:

   ```bash
   make clean
   ```

## Documentation and References

### Architecture Overview

- **Modular Design**:
  - `Chip8`: System coordinator
  - `CPU`, `Memory`, `Display`: Core functional modules
- **User Interface**:
  - Real-time visualization of memory, stack, and registers
  - Custom GDI+ rendering
  - ROM loader and UI buttons for control (Start, Pause, Reset, Load)
- **Input Handling**:
  - Fully supports the CHIP-8 hexadecimal keypad layout
- **Audio Feedback**:
  - Sound playback via WinMM

### Technical References

- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [CHIP-8 on Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [CHIP-8 Instruction Set](https://johnearnest.github.io/Octo/docs/chip8ref.pdf)
- [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [Games For CHIP-8](https://github.com/mir3z/chip8-emu/tree/master/roms)

---

## Project Purpose

This emulator was created as a practical exploration of low-level development, real-time graphics rendering, and system emulation using Windows-native technologies. It serves both as a hands-on learning tool and a demonstration of competency in systems programming and emulator architecture.

---

## License

This project is licensed under the MIT License. Contributions and feedback are welcome.
