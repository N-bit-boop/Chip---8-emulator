#include "Chip 8 Emulator.h"
#include "sdl_frontend.hpp"
#include <chrono>
#include <thread>
#include <iostream>
#include <map>
#include <string>
#include <set>


std::string MapToPC(uint8_t keyIndex) {
    static const std::map<uint8_t, std::string> keyMap = {
        {0x1, "1"}, {0x2, "2"}, {0x3, "3"}, {0xC, "4"},
        {0x4, "Q"}, {0x5, "W"}, {0x6, "E"}, {0xD, "R"},
        {0x7, "A"}, {0x8, "S"}, {0x9, "D"}, {0xE, "F"},
        {0xA, "Z"}, {0x0, "X"}, {0xB, "C"}, {0xF, "V"}
    };

    auto it = keyMap.find(keyIndex);
    if (it != keyMap.end()) {
        return it->second;
    }
    else {
        return "Unknown";
    }
}


std::set<uint8_t> detectUsedKeys(const Chip8& chip) {
    std::set<uint8_t> usedKeys;
    uint16_t pc = 0x200;
    uint16_t end = 4096;

    uint8_t lastSet[16] = { 0 }; // Track last LD Vx, kk value for each register

    while (pc < end - 1) {
        uint16_t opcode = (chip.memory[pc] << 8) | chip.memory[pc + 1];
        pc += 2;

        uint8_t x = (opcode >> 8) & 0xF;
        uint8_t kk = opcode & 0xFF;

        // Track key values loaded into Vx
        if ((opcode & 0xF000) == 0x6000) {
            lastSet[x] = kk;
        }

        // Detect key-check opcodes
        if ((opcode & 0xF0FF) == 0xE09E || (opcode & 0xF0FF) == 0xE0A1) {
            usedKeys.insert(lastSet[x]);
        }
    }

    return usedKeys;
}


void showMenu(SDLFrontend& frontend, const std::set<uint8_t>& usedKeys) {
    SDL_Event e;
    bool waiting = true;
    SDL_Renderer* renderer = frontend.getrender();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    std::cout << "\n=== CHIP-8 Emulator ===\n";
    std::cout << "Detected Controls:\n----------------------------\n";

    for (uint8_t k : usedKeys) {
        std::cout << "CHIP-8 Key " << std::hex << (int)k
            << "  ?  PC Key [" << MapToPC(k) << "]\n";
    }

    std::cout << "----------------------------\n";
    std::cout << "Press ENTER to start, ESC to quit\n";

    while (waiting) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                exit(0);
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN)
                    waiting = false;
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);
            }
        }
        SDL_Delay(10);
    }
}


int main(int argc, char* argv[]) {
    Chip8 chip;
    chip.reset();


    std::string romPath = "C:/Users/niran/source/repos/Chip 8 Emulator/roms/Airplane.ch8";
    chip.LoadROM(romPath.c_str());

    SDLFrontend frontend;
    frontend.init();

   
    auto usedKeys = detectUsedKeys(chip);
    showMenu(frontend, usedKeys);

    // Emulator loop setup
    const int INSTRUCTIONS_PER_FRAME = 1;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
