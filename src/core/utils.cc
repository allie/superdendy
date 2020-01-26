#include "core/utils.h"
#include <cstdlib>
#include <cstdio>

using namespace SuperDendy;
using namespace SuperDendy::Core;

BinaryFile::BinaryFile(
	Logger& logger,
	const char* file
) :
	logger(logger)
{
	std::ifstream ifs(file, std::ios::binary | std::ios::ate);

	if (ifs.good()) {
		logger.debug("Successfully opened binary file for reading: " + (std::string)file);
	} else {
		logger.error("Unable to open binary file for writing: " + (std::string)file);
		return;
	}

    size = ifs.tellg();
	logger.debug("File size: " + std::to_string(size));

    ifs.seekg(0, std::ios::beg);
    buffer.resize(size);
    ifs.read((char*)buffer.data(), size);

	logger.debug("Finished loading binary file");
}

BinaryFile::~BinaryFile() {
}

long int BinaryFile::get_size() {
	return size;
}
