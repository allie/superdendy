#pragma once

#include "common.h"
#include "emu/emulator.h"
#include "emu/famicom/cart.h"
#include "emu/famicom/cpu_bus.h"
#include "emu/mos6502/cpu.h"
#include "emu/mos6502/interrupt.h"

namespace SuperDendy::Emu::Famicom {
	class Emulator : public IEmulator {
	private:
		SuperDendy::Core::Config& config;
		SuperDendy::Core::Graphics& graphics;
		SuperDendy::Core::Audio& audio;
		SuperDendy::Core::Input& input;

		Cart cart;
		CPUBus cpu_bus;
		MOS6502::InterruptLine interrupt_line;
		MOS6502::CPU cpu;

		Dword step();

	public:
		Emulator(
			SuperDendy::Core::Config& config,
			SuperDendy::Core::Graphics& graphics,
			SuperDendy::Core::Audio& audio,
			SuperDendy::Core::Input& input,
			const char* file
		);
		~Emulator();

		void load_file(const char* file) override;
		Dim get_dimensions() override;
		void power_on() override;
		void reset() override;
		void emulate() override;
		void render() override;
	};
};
