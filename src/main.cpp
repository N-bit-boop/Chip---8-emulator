#include "Chip 8 Emulator.h"
#include "sdl_frontend.hpp"
#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char* argv[]) {
    Chip8 chip;
    chip.reset();

    
    chip.LoadROM("C:/Users/niran/source/repos/Chip 8 Emulator/roms/PONG2");

    SDLFrontend frontend;
    frontend.init();

    const int INSTRUCTIONS_PER_FRAME = 5;     
    const int TARGET_FPS = 60;
    const auto frameDuration = std::chrono::milliseconds(1000 / TARGET_FPS);

    using clock = std::chrono::high_resolution_clock;
    auto lastFrame = clock::now();

    while (true) {
        frontend.handleInput(chip);

        // Run a few instructions per frame (controls CPU speed)
        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; ++i) {
            chip.cycle();
        }

        frontend.draw(chip);

        // --- Update timers at 60 Hz ---
        auto now = clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrame);
        if (delta >= frameDuration) {
            if (chip.delayT > 0) chip.delayT--;
            if (chip.soundT > 0) {
                chip.soundT--;
                if (chip.soundT == 1) std::cout << "BEEP!\n";
            }
            lastFrame = now;
        }

        // --- Frame limiter ---
        std::this_thread::sleep_for(frameDuration - delta);
    }

    return 0;
}
