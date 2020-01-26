#include "emu/famicom/cpu_bus.h"

using namespace SuperDendy;
using namespace SuperDendy::Emu::Famicom;

CPUBus::CPUBus(
	SuperDendy::Core::Logger& logger
) :
	logger(logger),
	ram(logger, 0x800)
{
	logger.debug("CPU bus instantiated");
}

CPUBus::~CPUBus() {

}

Byte CPUBus::peek(Word addr) const {
	return ram.peek(addr);
}

Byte CPUBus::read(Word addr) {
	return ram.read(addr);
}

void CPUBus::write(Word addr, Byte val) {
	ram.write(addr, val);
}

int CPUBus::get_size() {
	return 0xFFFF;
}

