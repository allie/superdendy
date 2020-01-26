#include "emu/famicom/famicom.h"
#include "core/logger.h"

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu::Famicom;

FamicomEmulator::FamicomEmulator(
	SuperDendy::Core::Logger& logger,
	SuperDendy::Core::Config& config,
	SuperDendy::Core::Graphics& graphics,
	SuperDendy::Core::Audio& audio,
	SuperDendy::Core::Input& input,
	const char* file
) :
	Emulator(),
	logger(logger),
	config(config),
	graphics(graphics),
	audio(audio),
	input(input),
	cart(logger, file) {
	logger.info("Famicom emulator initialized");
}

FamicomEmulator::~FamicomEmulator() {
}

void FamicomEmulator::load_file(const char* file) {
	logger.info("Loaded file " + (std::string)file);
}

Dim FamicomEmulator::get_dimensions() {
	return {
		256,
		240
	};
}

void FamicomEmulator::power_on() {
	logger.info("Famicom powered on");
}

void FamicomEmulator::reset() {
	logger.info("Famicom reset");
}

void FamicomEmulator::emulate() {
	logger.info("Testing emulate()");
}

void FamicomEmulator::render() {
	graphics.clear();
	graphics.render_string(cart.name, 0, 0);
	graphics.present();
}
