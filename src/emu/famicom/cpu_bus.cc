#include "emu/famicom/cpu_bus.h"
#include "core/logger.h"

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu::Famicom;

CPUBus::CPUBus() : ram(0x800) {
	Logger::debug("CPU bus instantiated");
}

CPUBus::~CPUBus() {
}

Byte CPUBus::peek(Word addr) const {
	// Working RAM address range (mirrored every 2KB)
	if (in_range(addr, 0x0000, 0x1FFF)) {
		return ram.peek(addr & 0x07FF);
	}
	// PPU address range (mirrored every 8 bytes)
	if (in_range(addr, 0x2000, 0x3FFF)) {
		// return ppu.peek_cpu(0x2000 + (addr & 0x0007));
		return 0x00;
	}
	// Cartridge address range
	if (in_range(addr, 0x4020, 0xFFFF)) {
		// return cart.peek_cpu(addr);
		return 0x00;
	}
	// Invalid read operation or not yet implemented, just return 0
	return 0;
}

Byte CPUBus::read(Word addr) {
	// Working RAM address range (mirrored every 2KB)
	if (in_range(addr, 0x0000, 0x1FFF)) {
		return ram.read(addr & 0x07FF);
	}
	// PPU address range (mirrored every 8 bytes)
	if (in_range(addr, 0x2000, 0x3FFF)) {
		// return ppu.read_cpu(0x2000 + (addr & 0x0007));
		return 0x00;
	}
	// Controllers
	if (in_range(addr, 0x4016, 0x4017)) {
		// return controller.read(addr & 0x0001);
		return 0x00;
	}
	// Cartridge address range
	if (in_range(addr, 0x4020, 0xFFFF)) {
		// return cart.read_cpu(addr);
		return 0x00;
	}
	// Invalid read operation or not yet implemented
	Logger::debug("Invalid CPU read at address " + std::to_string(addr));
	return 0x00;
}

void CPUBus::write(Word addr, Byte val) {
	// Working RAM address range (mirrored every 2KB)
	if (in_range(addr, 0x0000, 0x1FFF)) {
		ram.write(addr & 0x07FF, val);
		return;
	}
	// PPU address range (mirrored every 8 bytes)
	if (in_range(addr, 0x2000, 0x3FFF)) {
		// ppu.write_cpu(0x2000 + (addr & 0x0007), val);
		return;
	}
	// Begin OAM DMA
	if (addr == 0x4014) {
		// cpu.suspend(cpu.cycles % 2 == 1 ? 514 : 513);
		// ppu.write_cpu(addr, val);
		return;
	}
	// Controllers
	if (in_range(addr, 0x4016, 0x4017)) {
		// controller.write(addr & 0x0001);
		return;
	}
	// Cartridge address range
	if (in_range(addr, 0x4020, 0xFFFF)) {
		// cart.write_cpu(addr, val);
		return;
	}
	// Invalid write operation or not yet implemented
	Logger::debug("Invalid CPU write at address " + std::to_string(addr) + ", value " + std::to_string(val));
}

int CPUBus::get_size() {
	return 0x10000;
}
