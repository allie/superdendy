#pragma once

#include "common.h"
#include <vector>

namespace SuperDendy::Core {
	class BinaryFile {
	private:
		long int size;

	public:
		BinaryFile(const char* file);
		~BinaryFile();

		long int get_size();

		std::vector<Byte> buffer;
	};
};
