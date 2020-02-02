#pragma once

#include "common/data.h"
#include "core/config.h"
#include <SDL2/SDL.h>
#include <string>

namespace SuperDendy::Core {
	class Input {
	private:
		Config& config;

	public:
		Input(Config& config, const std::string& db_path);
		~Input();

		void pressed(SDL_Keysym sym);
		void pressed(SDL_GameControllerButton button);
		void released(SDL_Keysym sym);
		void released(SDL_GameControllerButton button);
	};
};
