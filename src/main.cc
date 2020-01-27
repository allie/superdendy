#include "core/logger.h"
#include "core/config.h"
#include "core/input.h"
#include "core/graphics.h"
#include "core/audio.h"
#include "emu/emulator.h"

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu;

int main(int argc, char* argv[]) {
	Logger::start("sd.log", LogLevel::ALL);

	if (argc < 2) {
		Logger::info("No input file supplied");
		return 1;
	}

	Config config("sd.ini");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::fatal(SDL_GetError());
		return 1;
	}

	Graphics graphics(config);
	Audio audio(config);
	Input input(config, "gamecontrollerdb.txt");

	auto emulator = get_emulator_for_file(config, graphics, audio, input, argv[1]);

	if (emulator == nullptr) {
		Logger::fatal("No emulator available to load file");
		return 1;
	}

	graphics.create_window(emulator->get_dimensions());

	SDL_Thread* system_thread = SDL_CreateThread((SDL_ThreadFunction)emulate_callback, "system", emulator.get());
	if (system_thread == NULL) {
		Logger::fatal(SDL_GetError());
		return 1;
	}

	bool running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		emulator->render();
	}

	SDL_Quit();
	return 0;
}
