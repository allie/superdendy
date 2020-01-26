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
	Logger logger("sd.log", LogLevel::ALL);

	if (argc < 2) {
		logger.info("No input file supplied");
		return 1;
	}

	Config config(logger, "sd.ini");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logger.fatal(SDL_GetError());
		return 1;
	}

	Graphics graphics(logger, config);
	Audio audio(logger, config);
	Input input(logger, config, "gamecontrollerdb.txt");

	auto emulator = Emulator::from_file(logger, config, graphics, audio, input, argv[1]);

	if (emulator == nullptr) {
		logger.fatal("No emulator available to load file");
		return 1;
	}

	graphics.create_window(emulator->get_dimensions());

	SDL_Thread* system_thread = SDL_CreateThread((SDL_ThreadFunction)Emulator::callback, "system", emulator.get());
	if (system_thread == NULL) {
		logger.fatal(SDL_GetError());
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
