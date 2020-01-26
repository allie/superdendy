#pragma once

#include "common.h"
#include "emu/emulator.h"
#include "emu/famicom/cart.h"

namespace SuperDendy::Emu::Famicom {
	class FamicomEmulator : public Emulator {
	private:
		SuperDendy::Core::Logger& logger;
		SuperDendy::Core::Config& config;
		SuperDendy::Core::Graphics& graphics;
		SuperDendy::Core::Audio& audio;
		SuperDendy::Core::Input& input;
		Cart cart;

	public:
		FamicomEmulator(
			SuperDendy::Core::Logger& logger,
			SuperDendy::Core::Config& config,
			SuperDendy::Core::Graphics& graphics,
			SuperDendy::Core::Audio& audio,
			SuperDendy::Core::Input& input,
			const char* file
		);
		~FamicomEmulator();

		void load_file(const char* file) override;
		Dim get_dimensions() override;
		void power_on() override;
		void reset() override;
		void emulate() override;
		void render() override;
	};
};
