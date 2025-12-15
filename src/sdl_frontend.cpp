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
		
		if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
			bool isPressed = (e.type == SDL_KEYDOWN);
			switch (e.key.keysym.sym) {
			case SDLK_1: chip.keypad[0x1] = isPressed; break;
			case SDLK_2: chip.keypad[0x2] = isPressed; break;
			case SDLK_3: chip.keypad[0x3] = isPressed; break;
			case SDLK_4: chip.keypad[0xC] = isPressed; break;

			case SDLK_q: chip.keypad[0x1] = isPressed; break;
			case SDLK_w: chip.keypad[0x5] = isPressed; break;
			case SDLK_e: chip.keypad[0x6] = isPressed; break;
			case SDLK_r: chip.keypad[0xD] = isPressed; break;

			case SDLK_a: chip.keypad[0x4] = isPressed; break;
			case SDLK_s: chip.keypad[0x8] = isPressed; break;
			case SDLK_d: chip.keypad[0x9] = isPressed; break;
			case SDLK_f: chip.keypad[0xE] = isPressed; break;

			case SDLK_p: chip.keypad[0xC] = isPressed; break;
			case SDLK_l: chip.keypad[0xD] = isPressed; break;


			case SDLK_z: chip.keypad[0xA] = isPressed; break;
			case SDLK_x: chip.keypad[0x0] = isPressed; break;
			case SDLK_c: chip.keypad[0xB] = isPressed; break;
			case SDLK_v: chip.keypad[0xF] = isPressed; break;

			default: break;
			}
		}
	}
}

void SDLFrontend::shutdown() {
	if (texture)  SDL_DestroyTexture(texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window)   SDL_DestroyWindow(window);
	SDL_Quit();
}
