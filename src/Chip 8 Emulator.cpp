#include "Chip 8 Emulator.h"
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <filesystem>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START = 0x50;

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void Chip8::loadFont(const uint8_t* data, std::size_t n) {
	for (std::size_t i = 0; i < n; i++) {
		memory[FONTSET_START + i] = data[i];
	}
}


void Chip8::reset() {
	std::fill(std::begin(registers), std::end(registers), 0);
	std::fill(std::begin(stack), std::end(stack), 0);
	std::fill(std::begin(keypad), std::end(keypad), 0);
	std::fill(std::begin(video), std::end(video), 0);
	std::fill(std::begin(memory), std::end(memory), 0);

	index = 0;
	pc = P_START;   // 0x200
	sp = 0;
	delayT = 0;
	soundT = 0;
	opcode = 0;

	loadFont(fontset, 80);
}

void Chip8::LoadROM(const char* filename) {
	std::cout << "cwd: " << std::filesystem::current_path() << "\n";
	std::cout << "trying: " << filename << "\n";

	//open file, convert to binary and go to the end 
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		//size of file 
		std::streamsize size = file.tellg();
		if (size <= 0) {
			std::cout << "Empty, Invalid ROM" << std::endl;
			return;
		}

		if (size > (4096 - 0x200)) {
			std::runtime_error("ROM too large for memory");
			return;
		}

		char* buffer = new char[size];

		//go to the beginning
		file.seekg(0, std::ios::beg);
		file.read(buffer, size); // fill the mf buffer 
		if (file.gcount() != size) {
			std::cout << "short read";
			return;
		}
		file.close();

		//Load ROM into memory
		for (long i = 0; i < size; i++) {
			memory[0x200 + i] = buffer[i];
		}

		delete[] buffer;

	}
	else {
		std::cout << "Failed to open" << std::endl;
	}
}

uint16_t Chip8::fetch() {
	uint8_t hi = memory[pc];
	uint8_t lo = memory[pc + 1];
	uint16_t opcode = (hi << 8) | lo; // combine into 16 bit for instructions 
	pc += 2; // move forward vro..
	return opcode;
}

void Chip8::execute(uint16_t opcode) {

	uint8_t x = (opcode >> 8) & 0xF;
	uint8_t kk = opcode & 0xFF;
	uint8_t y = (opcode >> 4) & 0xF;
	uint16_t nnn = opcode & 0x0FFF;
	uint8_t last = opcode & 0xF;

	if ((opcode & 0xF000) == 0xE000) {
		std::cout << "Key opcode: " << std::hex << opcode
			<< " (checking key index V" << ((opcode >> 8) & 0xF)
			<< " = " << (int)registers[(opcode >> 8) & 0xF] << ")\n";
	}


	switch (opcode & 0xF000) {
	case 0x6000: { // LD Vx, kk
		registers[x] = kk;
		break;
	}

	case 0x1000: { // JP addr
		pc = nnn;           // absolute jump; fetch already advanced PC
		break;
	}

	case 0x7000: {
		registers[x] = (registers[x] + kk) & 0xFF; // dat jawnforce to wrap 8 bits 
		break;
	}

	case 0xA000: {
		index = nnn;
		break;
	}

	case 0x2000: { // CALL addr
		stack[sp] = pc;
		sp++;
		pc = nnn;
		break;
	}

	case 0x0000: { // System instructions
		switch (opcode) {
		case 0x00EE: { // RET
			pc = stack[--sp];
			break;
		}
		case 0x00E0: { // CLS
			std::fill(std::begin(video), std::end(video), 0);
			break;
		}
		}
		break;
	}

	case 0x3000: { // SE Vx, kk
		if (registers[x] == kk) {
			pc += 2;
		}
		break;
	}

	case 0x4000: { // SNE Vx, kk
		if (registers[x] != kk) {
			pc += 2;
		}
		break;
	}

	case 0x5000: { // SE Vx, Vy
		if ((opcode & 0xF) == 0) {
			if (registers[x] == registers[y]) {
				pc += 2;
			}
		}
		break;
	}

	case 0x8000: {
		switch (last) {
		case 0x0: { registers[x] = registers[y]; break; }
		case 0x1: { registers[x] |= registers[y]; break; }
		case 0x2: { registers[x] &= registers[y]; break; }
		case 0x3: { registers[x] ^= registers[y]; break; }
		case 0x4: {
			uint16_t sum = registers[x] + registers[y];
			registers[0xF] = (sum > 255);
			registers[x] = sum & 0xFF;
			break;
		}
		case 0x5: { // SUB
			registers[0xF] = (registers[x] >= registers[y]);
			registers[x] -= registers[y];
			break;
		}
		case 0x6: {
			registers[0xF] = registers[x] & 0x1;
			registers[x] >>= 1;
			break;
		}
		case 0x7: {
			registers[0xF] = (registers[y] >= registers[x]);
			registers[x] = registers[y] - registers[x];
			break;
		}
		case 0xE: {
			registers[0xF] = (registers[x] & 0x80) >> 7;
			registers[x] <<= 1;
			break;
		}
		default:
			std::cout << "Unknown 8xy* opcode: " << std::hex << opcode << std::endl;
			break;
		}
		break;
	}
	case 0xC000: { // RND Vx, byte
		uint8_t rnd = rand() % 256;   // random 0–255
		registers[x] = rnd & kk;
		break;
	}


	case 0xD000: { // DRW Vx, Vy, nibble
		uint8_t x = (opcode >> 8) & 0xF;
		uint8_t y = (opcode >> 4) & 0xF;
		uint8_t n = opcode & 0xF;

		uint8_t xPos = registers[x] % W;
		uint8_t yPos = registers[y] % H;

		//reset collision flag 
		registers[0xF] = 0;

		for (std::size_t row{ 0 }; row < n; ++row) {
			uint8_t spriteByte = memory[index + row];
			for (std::size_t col{ 0 }; col < 8; ++col) {
				if ((spriteByte & (0x80 >> col)) != 0) {
					std::size_t pixelX = (xPos + col) % W;
					std::size_t pixelY = (yPos + row) % H;
					std::size_t pixelIndex = pixelY * W + pixelX;

					//collision checks 
					if (video[pixelIndex] == 0xFFFFFFFF)
						registers[0xF] = 1;

					// XOR toggle pixel on/off
					video[pixelIndex] ^= 0xFFFFFFFF;
				}
			}
		}
		break;
	}
	case 0xE000: {
		switch (kk) {
		case 0x9E:
			if (keypad[registers[x]] != 0) {
				pc += 2;
				break;
			}
		case 0xA1:
			if (keypad[registers[x]] == 0) {
				pc += 2;
			}
			break;
		}
		break;
	}
	case 0xF000: {
		switch (kk) {
		case 0x07: // LD Vx, DT
			registers[x] = delayT;
			break;

		case 0x15: // LD DT, Vx
			delayT = registers[x];
			break;

		case 0x18: // LD ST, Vx
			soundT = registers[x];
			break;

		case 0x1E: // ADD I, Vx
			index += registers[x];
			break;

		case 0x29: // LD F, Vx
			index = FONTSET_START + (5 * registers[x]); // 5 bytes per font
			break;

		case 0x33: { // LD B, Vx (binary-coded decimal)
			uint8_t value = registers[x];
			memory[index + 0] = value / 100;
			memory[index + 1] = (value / 10) % 10;
			memory[index + 2] = value % 10;
			break;
		}

		case 0x55: // LD [I], Vx (store registers V0..Vx in memory)
			for (int i = 0; i <= x; ++i)
				memory[index + i] = registers[i];
			break;

		case 0x65: // LD Vx, [I] (load registers V0..Vx from memory)
			for (int i = 0; i <= x; ++i)
				registers[i] = memory[index + i];
			break;

		default:
			std::cout << "Unknown Fx** opcode: " << std::hex << opcode << std::endl;
			break;
		}
		break;
	}

	default:
		std::cout << "Unknown opcode: 0x" << std::hex << opcode << std::dec << "\n";
		break;
	}
}

void Chip8::cycle() {
	uint16_t op = fetch();
	execute(op);
	
}
