#include "emu/famicom/cart.h"
#include "core/utils.h"
#include "core/logger.h"

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu::Famicom;

// Construct a Cart with rom.buffer loaded from a .nes file
Cart::Cart(const char* file) {
	BinaryFile rom(file);

	Logger::debug("Read ROM file to memory");

	// Check for NES[EOF]
	if (rom.buffer[0] != 'N' || rom.buffer[1] != 'E' || rom.buffer[2] != 'S' || rom.buffer[3] != 0x1A) {
		Logger::fatal("Invalid NES ROM file");
		throw std::runtime_error("Invalid NES ROM file");
	}

	name = std::string(file);
	Logger::debug("ROM name: " + name);

	prg_rom_size = rom.buffer[4];
	Logger::debug("PRG ROM size: " + std::to_string(prg_rom_size));

	chr_rom_size = rom.buffer[5];
	Logger::debug("CHR ROM size: " + std::to_string(chr_rom_size));

	prg_ram_size = rom.buffer[8];
	Logger::debug("PRG RAM size: " + std::to_string(prg_ram_size));

	// Flags 6
	if (!(rom.buffer[6] & 0x01) && !((rom.buffer[6] & 0x08) >> 3)) {
		vram_arr = V_MIRROR;
		Logger::debug("VRAM arrangement: V_MIRROR");
	} else if (!(rom.buffer[6] & 0x01) && ((rom.buffer[6] & 0x08) >> 3)) {
		vram_arr = H_MIRROR;
		Logger::debug("VRAM arrangement: H_MIRROR");
	} else if (rom.buffer[6] & 0x01) {
		vram_arr = Q_SCREEN;
		Logger::debug("VRAM arrangement: Q_SCREEN");
	}

	has_sram = (rom.buffer[6] & 0x02) >> 1;
	Logger::debug("Has SRAM: " + std::to_string(has_sram));

	has_trainer = (rom.buffer[6] & 0x04) >> 2;
	Logger::debug("Has trainer: " + std::to_string(has_trainer));

	// Flags 7
	vs_unisystem = rom.buffer[7] & 0x01;
	Logger::debug("VS UniSystem: " + std::to_string(vs_unisystem));

	playchoice = (rom.buffer[7] & 0x02) >> 1;
	Logger::debug("PlayChoice: " + std::to_string(playchoice));

	nes2 = ((rom.buffer[7] & 0x0C) >> 2) == 2;
	Logger::debug("INES 2.0: " + std::to_string(nes2));

	// Mapper number (flags 6 and 7)
	mapper_num = ((rom.buffer[6] & 0xF0) >> 4) | (rom.buffer[7] & 0xF0);
	Logger::debug("Mapper: " + std::to_string(mapper_num));

	// Flags 9
	tv_mode = rom.buffer[9] & 0x01;
	if (tv_mode == NTSC) {
		Logger::debug("TV mode: NTSC");
	} else if (tv_mode == PAL) {
		Logger::debug("TV mode: PAL");
	}

	// Flags 10 is ignored

	// Load trainer rom.buffer into memory if necessary
	trainer = new Byte[512];
	if (has_trainer) {
		memcpy(trainer, rom.buffer.data() + 16, 512 * sizeof(Byte));
	}

	// Load PRG ROM into memory
	prg = new Byte[0x4000 * prg_rom_size];
	memcpy(
		prg,
		rom.buffer.data() + 16 + (has_trainer * 512),
		0x4000 * prg_rom_size * sizeof(Byte)
	);

	// Load CHR ROM into memory if necessary
	chr = new Byte[0x2000 * (chr_rom_size != 0 ? chr_rom_size : 1)];
	if (chr_rom_size) {
		memcpy(
			chr,
			rom.buffer.data() + 16 + (has_trainer * 512) + (0x4000 * prg_rom_size),
			0x2000 * chr_rom_size * sizeof(Byte)
		);
	}

	sram = new Byte[0x2000];

	// Initialize mapper
	mapper = Mapper::by_number(mapper_num);
}

Cart::~Cart() {
	delete[] trainer;
	delete[] prg;
	delete[] chr;
	delete[] sram;
}

Byte Cart::read_cpu(Word addr) {
	return mapper->read_cpu(addr);
}

void Cart::write_cpu(Word addr, Byte val) {
	mapper->write_cpu(addr, val);
}

Byte Cart::peek_cpu(Word addr) {
	return mapper->peek_cpu(addr);
}

Byte Cart::read_ppu(Word addr) {
	return mapper->read_ppu(addr);
}

void Cart::write_ppu(Word addr, Byte val) {
	mapper->write_ppu(addr, val);
}

Byte Cart::peek_ppu(Word addr) {
	return mapper->peek_ppu(addr);
}
