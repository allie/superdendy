#include "emu/emulator.h"
#include "emu/famicom/famicom.h"
#include <filesystem>

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu;
using namespace SuperDendy::Emu::Famicom;

std::unique_ptr<IEmulator> Emu::get_emulator_for_file(
	SuperDendy::Core::Config& config,
	SuperDendy::Core::Graphics& graphics,
	SuperDendy::Core::Audio& audio,
	SuperDendy::Core::Input& input,
	const char* file
) {
	std::filesystem::path filepath(file);
	auto extension = filepath.extension();

	if (extension == ".nes") {
		return std::unique_ptr<IEmulator>(new Famicom::Emulator(config, graphics, audio, input, file));
	}

	return nullptr;
}

// SDL_Thread callback
void Emu::emulate_callback(void* data) {
	auto emulator = static_cast<IEmulator*>(data);
	emulator->emulate();
}
