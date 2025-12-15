#include "Chip 8 Emulator.h"
#include "sdl_frontend.hpp"
#include <chrono>
#include <thread>
#include <iostream>

int SDL_main(int argc, char* argv[]) {
    Chip8 chip;
    chip.reset();

    chip.LoadROM("C:/Users/niran/source/repos/Chip 8 Emulator/roms/PONG2");

    SDLFrontend sdl;

    if (!sdl.init()) {
        return -1;
    }
    const int cycleRate = 500; // 500 Hz (500 instructions per second)
    const int msPerCycle = 1000 / cycleRate;

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        chip.cycle(); // run one instruction

        sdl.handleInput(chip);
        sdl.draw(chip);
        // display later

        // maintain constant speed
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        if (elapsed.count() < msPerCycle) {
            std::this_thread::sleep_for(std::chrono::milliseconds(msPerCycle) - elapsed);
        }
    }

    return 0;
}
