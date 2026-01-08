# CHIP-8 Emulator
CHIP-8 emulator written in C++ with an SDL2-based frontend to execute and render classic CHIP-8 programs (ROMs).  
Implements the full fetch–decode–execute cycle, opcode handling, keyboard input mapping, and real-time graphics rendering.

---

### 1. Build Requirements

- C++17 or later
- SDL2
- A CHIP-8 ROM file (.ch8)

Ensure SDL2 is installed and properly linked.

---

### 2. Compile the Emulator

Example using g++ on Windows/Linux:

```bash
g++ *.cpp -o chip8 -lSDL2
⚠️ Compilation flags may vary depending on your SDL2 installation.

3. Load a ROM
Update the ROM path inside main.cpp:

cpp
Copy code
std::string romPath = "path/to/your/rom.ch8";
chip.LoadROM(romPath.c_str());
4. Run the Emulator
bash
Copy code
./chip8
The emulator will automatically detect which CHIP-8 keys are used and display a control menu.

5. Controls
CHIP-8 → Keyboard Mapping

CHIP-8 Key	PC Key
1 2 3 C	1 2 3 4
4 5 6 D	Q W E R
7 8 9 E	A S D F
A 0 B F	Z X C V

Other Controls:

ENTER → Start execution

ESC → Quit emulator

6. Project Structure
pgsql
Copy code
Chip 8 Emulator.cpp / .h   → Core CPU (memory, registers, opcodes, timers)
sdl_frontend.cpp / .h      → SDL2-based rendering & input handling
main.cpp                   → Emulator loop, timing, ROM loading, key detection
7. Technical Highlights
Implements all major CHIP-8 opcodes (0x0NNN – 0xFxxx)

Accurate sprite rendering using XOR pixel toggling

Collision detection via VF register

Instruction timing decoupled from frame rate

Clean separation between CPU logic and frontend rendering

8. Future Improvements
Sound output using SDL audio

Configurable key bindings

ROM selection menu

Debugger / step-through execution

Save/load emulator state

vbnet
Copy code
