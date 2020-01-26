#include "core/input.h"

using namespace SuperDendy::Core;

Input::Input(
	Logger& logger,
	Config& config,
	const std::string& db_path
) :
	logger(logger),
	config(config)
{
}

Input::~Input() {
}

void Input::pressed(SDL_Keysym sym) {

}

void Input::pressed(SDL_GameControllerButton button) {

}

void Input::released(SDL_Keysym sym) {

}

void Input::released(SDL_GameControllerButton button) {

}
