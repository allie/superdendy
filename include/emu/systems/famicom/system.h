#pragma once

#include "common/data.h"
#include "emu/emulator.h"
#include "emu/systems/famicom/cart.h"
#include "emu/systems/famicom/cpu_bus.h"
#include "emu/devices/mos6502/cpu.h"
#include "emu/devices/mos6502/interrupt.h"

namespace SuperDendy::Emu::Famicom {
	class System : public Emulator {
	private:
		Core::Config& config;
		Core::Graphics& graphics;
		Core::Audio& audio;
		Core::Input& input;

		Cart cart;
		CPUBus cpu_bus;
		MOS6502::InterruptLine interrupt_line;
		MOS6502::CPU cpu;

		Dword step();
		void step_seconds(double dt);
		void step_frame();

	public:
		System(
			Core::Config& config,
			Core::Graphics& graphics,
			Core::Audio& audio,
			Core::Input& input
		);
		~System();

		bool test_file(BinaryFile& file) override;
		void load_file(BinaryFile& file) override;
		Dim get_dimensions() override;
		void power_on() override;
		void reset() override;
		void emulate() override;
		void render() override;
	};
};
