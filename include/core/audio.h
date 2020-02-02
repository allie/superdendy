#pragma once

#include "common/data.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <queue>

namespace SuperDendy::Core {
	class Audio {
	private:
		Config& config;
		std::queue<Sword*> sample_queue;
		SDL_AudioSpec* audio;
		Sword* samples;
		Sword* last_chunk;
		int current;
		int sample_size;

		void callback(void* userdata, Uint8* stream, int len);

	public:
		Audio(Config& config);
		~Audio();

		void add_sample(Sword sample);
	};
};
