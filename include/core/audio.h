#pragma once

#include "common.h"
#include "logger.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <queue>

namespace SuperDendy::Core {
	class Audio {
	private:
		Logger& logger;
		Config& config;
		std::queue<Sword*> sample_queue;
		SDL_AudioSpec* audio;
		Sword* samples;
		Sword* last_chunk;
		int current;
		int sample_size;

		void callback(void* userdata, Uint8* stream, int len);

	public:
		Audio(Logger& logger, Config& config);
		~Audio();

		void add_sample(Sword sample);
	};
};
