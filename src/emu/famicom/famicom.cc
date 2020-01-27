#include "emu/famicom/famicom.h"
#include "core/logger.h"

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu::Famicom;

Emulator::Emulator(
	SuperDendy::Core::Config& config,
	SuperDendy::Core::Graphics& graphics,
	SuperDendy::Core::Audio& audio,
	SuperDendy::Core::Input& input,
	const char* file
) :
	config(config),
	graphics(graphics),
	audio(audio),
	input(input),
	cart(file),
	cpu_bus(),
	cpu(cpu_bus, interrupt_line, MOS6502::Mode::NES)
{
	Logger::info("Famicom emulator initialized");
}

Emulator::~Emulator() {
}

void Emulator::load_file(const char* file) {
	Logger::info("Loaded file " + (std::string)file);
}

Dim Emulator::get_dimensions() {
	return {
		256,
		240
	};
}

void Emulator::power_on() {
	Logger::info("Famicom powered on");
}

void Emulator::reset() {
	Logger::info("Famicom reset");
}

Dword Emulator::step() {
	Dword cycles = cpu.step();
	return cycles;
}

void Emulator::emulate() {
	while (2) {
		step();
	}
}

void Emulator::render() {
	graphics.clear();
	graphics.render_string(cart.name, 0, 0);
	graphics.present();
}
