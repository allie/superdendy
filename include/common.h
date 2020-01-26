#pragma once

#include <cstdint>
#include <SDL2/SDL.h>

namespace SuperDendy {
	typedef uint8_t Byte;
	typedef uint16_t Word;
	typedef uint32_t Dword;
	typedef uint64_t Qword;
	typedef int16_t Sword;

	inline bool in_range(int val, int min, int max) {
		return val >= min && val <= max;
	}

	struct Dim {
		int w;
		int h;
	};

	struct Point {
		int x;
		int y;
	};

	struct RGB {
		Byte r;
		Byte g;
		Byte b;
	};

	struct RGBA {
		Byte r;
		Byte g;
		Byte b;
		Byte a;
	};

	struct Button {
		SDL_Keysym sym;
		SDL_GameControllerButton button;
	};
};
