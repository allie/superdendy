#include "emu/emulator.h"
#include "emu/famicom/famicom.h"
#include <filesystem>

using namespace SuperDendy;
using namespace SuperDendy::Emu;

SimpleRAM8::SimpleRAM8(
	SuperDendy::Core::Logger& logger,
	int size
) :
	logger(logger),
	size(size)
{
	data = new Byte[size];
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
