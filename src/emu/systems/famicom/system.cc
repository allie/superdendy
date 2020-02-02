#include "emu/systems/famicom/system.h"
#include "common/utils.h"
#include "core/logger.h"
#include <iostream>
#include <cstring>

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu::Famicom;

System::System(
	SuperDendy::Core::Config& config,
	SuperDendy::Core::Graphics& graphics,
	SuperDendy::Core::Audio& audio,
	SuperDendy::Core::Input& input
) :
	config(config),
	graphics(graphics),
	audio(audio),
	input(input),
	cart(),
	cpu_bus(cart),
	interrupt_line(),
	cpu(cpu_bus, interrupt_line, MOS6502::Mode::NES)
{
	Logger::info("Famicom emulator initialized");
}

System::~System() {
	// Destructor
}

bool System::test_file(BinaryFile& file) {
	Logger::info("testing file for nes");
	Logger::debug(std::to_string(file.buffer[0]));
	return file.buffer[0] == 'N' &&
		file.buffer[1] == 'E' &&
		file.buffer[2] == 'S' &&
		file.buffer[3] == 0x1A;
}

void System::load_file(BinaryFile& file) {
	cart.load_file(file);
}

Dim System::get_dimensions() {
	return {
		256,
		240
	};
}

void System::power_on() {
	Logger::info("Famicom powered on");
	cpu.power_on();
}

void System::reset() {
	Logger::info("Famicom reset");
	cpu.reset();
}

Dword System::step() {
	Dword cycles = cpu.step();

	for (int i = 0; i < cycles * 3; i++) {
		// ppu.step();
		cart.step();
	}

	return cycles;
}

void System::step_seconds(double dt) {
	int cycles = (int)(MOS6502::CLOCK_SPEED * dt);

	while (cycles > 0) {
		cycles -= step();
	}
	cpu_bus.dump("cpu_bus.bin");
}

void System::step_frame() {
	// int current_frame = ppu.frame;

	// while (ppu.frame == current_frame) {
	// 	step();
	// }
}

void System::emulate() {
	timespec current_time;
	timespec new_time;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time);

	while (running) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &new_time);
		timespec dt = clock_diff(current_time, new_time);

		step_seconds((double)dt.tv_nsec / 1000000000.0);

		memcpy(&current_time, &new_time, sizeof(struct timespec));
	}
}

void System::render() {
	graphics.clear();
	graphics.render_string(cart.name, 0, 0);
	graphics.present();
}
