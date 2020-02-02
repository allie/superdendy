#pragma once

#include "common/data.h"
#include "common/binary_file.h"
#include "core/config.h"
#include "core/graphics.h"
#include "core/audio.h"
#include "core/input.h"
#include <string>
#include <memory>

namespace SuperDendy::Emu {
	class Emulator {
	public:
		virtual ~Emulator() {};

		virtual bool test_file(BinaryFile& file) = 0;
		virtual void load_file(BinaryFile& file) = 0;
		virtual Dim get_dimensions() = 0;
		virtual void power_on() = 0;
		virtual void reset() = 0;
		virtual void emulate() = 0;
		virtual void render() = 0;
	};

	// Factory
	std::shared_ptr<Emulator> get_emulator_for_file(
		Core::Config& config,
		Core::Graphics& graphics,
		Core::Audio& audio,
		Core::Input& input,
		const char* file
	);

	// SDL_Thread callback
	void emulate_callback(void* data);
}
