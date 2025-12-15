#include "sdl_frontend.hpp"
#include<iostream>

SDLFrontend::SDLFrontend() {
	//empty constructor 
}

SDLFrontend::~SDLFrontend() {
	shutdown();
}

bool SDLFrontend::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not init! SDL_ERROR: " << SDL_GetError() << "\n";
		return false;
	}

	window = SDL_CreateWindow("Chip-8 Emulator",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Chip8::W * scale, Chip8::H * scale,
		SDL_WINDOW_SHOWN);
	
	if (!window) {
		std::cerr << "Window could not be created! SDLERROR: " << SDL_GetError() << "\n";
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Chip8::W, Chip8::H);
	return true;

}

void SDLFrontend::draw(const Chip8& chip) {
	SDL_UpdateTexture(texture, nullptr, chip.video, Chip8::W * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void SDLFrontend::handleInput(Chip8& chip) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			exit(0);
		}
		// (keyboard handling will be added later)
	}
}

void SDLFrontend::shutdown() {
	if (texture)  SDL_DestroyTexture(texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window)   SDL_DestroyWindow(window);
	SDL_Quit();
}
