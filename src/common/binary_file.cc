#include "common/binary_file.h"
#include "core/logger.h"
#include <iostream>
#include <fstream>

using namespace SuperDendy;
using namespace SuperDendy::Core;

BinaryFile::BinaryFile(
	const char* file
) :
	path(file)
{
	std::ifstream ifs(file, std::ios::binary | std::ios::ate);

	if (ifs.good()) {
		Logger::debug("Successfully opened binary file for reading: " + (std::string)file);
	} else {
		Logger::error("Unable to open binary file for writing: " + (std::string)file);
		return;
	}

    size = ifs.tellg();
	Logger::debug("File size: " + std::to_string(size));

    ifs.seekg(0, std::ios::beg);
    buffer.resize(size);
    ifs.read((char*)buffer.data(), size);

	Logger::debug("Finished loading binary file");
}

BinaryFile::~BinaryFile() {
}

long int BinaryFile::get_size() {
	return size;
}

std::string BinaryFile::get_path() {
	return path;
}
