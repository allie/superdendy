#pragma once

#include "common.h"
#include "core/logger.h"
#include <vector>

namespace SuperDendy::Core {
	class BinaryFile {
	private:
		Logger& logger;
		long int size;

	public:
		BinaryFile(Logger& logger, const char* file);
		~BinaryFile();

		long int get_size();

		std::vector<Byte> buffer;
	};
};
