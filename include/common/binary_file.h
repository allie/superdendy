#pragma once

#include "common/data.h"
#include <string>
#include <vector>
#include <time.h>

namespace SuperDendy {
	class BinaryFile {
	private:
		long int size;
		std::string path;

	public:
		BinaryFile(const char* file);
		~BinaryFile();

		long int get_size();
		std::string get_path();

		std::vector<Byte> buffer;
	};
};
