#include "emu/memory.h"
#include "core/logger.h"
#include <filesystem>
#include <fstream>

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu;

void Memory8::dump(const char* file) {
	// Logger::debug("Dumping memory to file " + (std::string)file);

	int size = get_size();
	// Logger::debug("Output file size: " + std::to_string(get_size()));

	Byte* buf = new Byte[size];
	for (int i = 0; i < size; i++) {
		buf[i] = peek(i);
	}

	std::ofstream ofs(file, std::ios::out | std::ios::binary);

	if (ofs.good()) {
		ofs.write((char*)buf, size);
		ofs.close();
	}
}

SimpleRAM8::SimpleRAM8(int size) : size(size) {
	data = new Byte[size];
}

SimpleRAM8::SimpleRAM8(Byte* initial_data, int size) : SimpleRAM8(size) {
	std::copy(initial_data, initial_data + size, data);
}

SimpleRAM8::~SimpleRAM8() {
	delete[] data;
}

Byte SimpleRAM8::peek(Word addr) const {
	return data[addr];
}

Byte SimpleRAM8::read(Word addr) {
	return data[addr];
}

void SimpleRAM8::write(Word addr, Byte val) {
	data[addr] = val;
}

int SimpleRAM8::get_size() {
	return size;
}
