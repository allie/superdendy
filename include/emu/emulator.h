#pragma once

#include "common.h"
#include "core/logger.h"
#include "core/config.h"
#include "core/graphics.h"
#include "core/audio.h"
#include "core/input.h"
#include <string>
#include <memory>

namespace SuperDendy::Emu {
	class Emulator {
	public:
		Emulator() {};
		virtual ~Emulator() {};

		virtual void load_file(const char* file) = 0;
		virtual Dim get_dimensions() = 0;
		virtual void power_on() = 0;
		virtual void reset() = 0;
		virtual void emulate() = 0;
		virtual void render() = 0;

		static std::unique_ptr<Emulator> from_file(
			SuperDendy::Core::Logger& logger,
			SuperDendy::Core::Config& config,
			SuperDendy::Core::Graphics& graphics,
			SuperDendy::Core::Audio& audio,
			SuperDendy::Core::Input& input,
			const char* file
		);
		static void callback(void* data);
	};
}
