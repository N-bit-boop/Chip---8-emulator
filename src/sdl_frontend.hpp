#pragma once
#include <SDL.h>
#include <iostream>
#include "Chip 8 Emulator.h"

class SDLFrontend {
public:
    SDLFrontend();
    ~SDLFrontend();

    bool init();
    void draw(const Chip8& chip);
    void handleInput(Chip8& chip);
    void shutdown();
    void runTestWindow();
    SDL_Renderer* getrender() const { return renderer;}

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    const int scale = 10; //Each pixel is a 10 by 10 block
};
