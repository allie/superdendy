#include "emu/emulator.h"
#include "emu/systems/famicom/system.h"
#include "core/logger.h"
#include <filesystem>

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu;
using namespace SuperDendy::Emu::Famicom;

std::shared_ptr<Emulator> Emu::get_emulator_for_file(
	SuperDendy::Core::Config& config,
	SuperDendy::Core::Graphics& graphics,
	SuperDendy::Core::Audio& audio,
	SuperDendy::Core::Input& input,
	const char* file
) {
	BinaryFile bin(file);

	std::filesystem::path filepath(file);
	auto extension = filepath.extension();

	if (extension == ".nes") {
		std::unique_ptr<Emulator> famicom(new Famicom::System(config, graphics, audio, input));
		famicom->load_file(bin);
		return famicom;
	}

	// else if (extension == ".ch8")

	return nullptr;
}

// SDL_Thread callback
void Emu::emulate_callback(void* data) {
	auto emulator = static_cast<Emulator*>(data);
	emulator->emulate();
}
