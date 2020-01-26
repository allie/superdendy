#pragma once

#include "common.h"
#include "logger.h"
#include <map>
#include <string>

namespace SuperDendy::Core {
	class Config {
	private:
		Logger& logger;
		std::string file;

		Qword pack_keysym(SDL_Keysym sym);
		SDL_Keysym unpack_keysym(Qword val);

	public:
		std::map<std::string, std::string> mappings;
		std::map<std::string, Button> keybindings;
		// TODO: controller bindings
		Point window_pos;
		int window_scale;
		float volume;

		Config(Logger& logger, const std::string& file);
		~Config();

		void load();
		void write();
		void load_defaults();
	};
};
