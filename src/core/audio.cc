#include "core/audio.h"
#include "core/logger.h"
#include <string>
#include <stdexcept>

using namespace SuperDendy::Core;

Audio::Audio(
	Config& config
) :
	config(config),
	current(0),
	sample_size(1024)
{
}

Audio::~Audio() {
}

void Audio::callback(void* userdata, Uint8* stream, int len) {
}

void Audio::add_sample(Sword sample) {
}
