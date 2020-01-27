#pragma once

#include "common.h"
#include "core/config.h"
#include "core/graphics.h"
#include "core/audio.h"
#include "core/input.h"
#include <string>
#include <memory>

namespace SuperDendy::Emu {
	class IEmulator {
	public:
		virtual ~IEmulator() {};

		virtual void load_file(const char* file) = 0;
		virtual Dim get_dimensions() = 0;
		virtual void power_on() = 0;
		virtual void reset() = 0;
		virtual void emulate() = 0;
		virtual void render() = 0;
	};

	std::unique_ptr<IEmulator> get_emulator_for_file(
		SuperDendy::Core::Config& config,
		SuperDendy::Core::Graphics& graphics,
		SuperDendy::Core::Audio& audio,
		SuperDendy::Core::Input& input,
		const char* file
	);

	void emulate_callback(void* data);
}
