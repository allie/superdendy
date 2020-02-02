#include "emu/systems/famicom/cart.h"
#include "core/logger.h"
#include <iostream>

using namespace SuperDendy;
using namespace SuperDendy::Core;
using namespace SuperDendy::Emu::Famicom;

// Load a .nes file
void Cart::load_file(BinaryFile& file) {
	Logger::debug("Read ROM file to memory");
	int cursor = 0;

	name = file.get_path();
	Logger::debug("ROM name: " + name);

	cursor = 4;

	prg_rom_size = file.buffer[cursor++];
	Logger::debug("PRG ROM size: " + std::to_string(prg_rom_size));

	chr_rom_size = file.buffer[cursor++];
	Logger::debug("CHR ROM size: " + std::to_string(chr_rom_size));

	prg_ram_size = file.buffer[cursor++];
	Logger::debug("PRG RAM size: " + std::to_string(prg_ram_size));

	// Flags 6
	cursor = 6;
	if (!(file.buffer[cursor] & 0x01) && !((file.buffer[cursor] & 0x08) >> 3)) {
		vram_arr = V_MIRROR;
		Logger::debug("VRAM arrangement: V_MIRROR");
	} else if (!(file.buffer[cursor] & 0x01) && ((file.buffer[cursor] & 0x08) >> 3)) {
		vram_arr = H_MIRROR;
		Logger::debug("VRAM arrangement: H_MIRROR");
	} else if (file.buffer[cursor] & 0x01) {
		vram_arr = Q_SCREEN;
		Logger::debug("VRAM arrangement: Q_SCREEN");
	}

	has_sram = (file.buffer[cursor] & 0x02) >> 1;
	Logger::debug("Has SRAM: " + std::to_string(has_sram));

	has_trainer = (file.buffer[cursor] & 0x04) >> 2;
	Logger::debug("Has trainer: " + std::to_string(has_trainer));

	// Flags 7
	cursor = 7;
	vs_unisystem = file.buffer[cursor] & 0x01;
	Logger::debug("VS UniSystem: " + std::to_string(vs_unisystem));

	playchoice = (file.buffer[cursor] & 0x02) >> 1;
	Logger::debug("PlayChoice: " + std::to_string(playchoice));

	nes2 = ((file.buffer[cursor] & 0x0C) >> 2) == 2;
	Logger::debug("INES 2.0: " + std::to_string(nes2));

	// Mapper number (flags 6 and 7)
	mapper_num = ((file.buffer[cursor - 1] & 0xF0) >> 4) | (file.buffer[cursor] & 0xF0);
	Logger::debug("Mapper: " + std::to_string(mapper_num));

	// Flags 9
	cursor = 9;
	tv_mode = file.buffer[cursor] & 0x01;
	if (tv_mode == NTSC) {
		Logger::debug("TV mode: NTSC");
	} else if (tv_mode == PAL) {
		Logger::debug("TV mode: PAL");
	}

	// Flags 10 is ignored

	// Past header, on to data
	cursor = 16;

	// Load trainer file.buffer into memory if necessary
	if (has_trainer) {
		trainer = std::make_unique<SimpleRAM8>(file.buffer.data() + cursor, 512);
		cursor += 512;
	} else {
		trainer = std::make_unique<SimpleRAM8>(512);
	}

	// Load PRG ROM into memory
	prg = std::make_unique<SimpleRAM8>(file.buffer.data() + cursor, 0x4000 * prg_rom_size);
	cursor += (0x4000 * prg_rom_size);

	// Load CHR ROM into memory if necessary
	chr = std::make_unique<SimpleRAM8>(file.buffer.data() + cursor, 0x2000 * chr_rom_size);
	cursor += (0x2000 * chr_rom_size);

	sram = std::make_unique<SimpleRAM8>(0x2000);

	// Initialize mapper
	mapper = Mapper::by_number(mapper_num);
}

Cart::~Cart() {
	// Destructor
}

Byte Cart::read_cpu(Word addr) {
	// return mapper->read_cpu(addr);
	return prg->read(addr & (prg_rom_size < 2 ? 0x3FFF : 0x7FFF));
}

void Cart::write_cpu(Word addr, Byte val) {
	// mapper->write_cpu(addr, val);
	prg->write(addr & (prg_rom_size < 2 ? 0x3FFF : 0x7FFF) , val);
}

Byte Cart::peek_cpu(Word addr) {
	// return mapper->peek_cpu(addr);
	return prg->peek(addr & (prg_rom_size < 2 ? 0x3FFF : 0x7FFF));
}

Byte Cart::read_ppu(Word addr) {
	// return mapper->read_ppu(addr);
	return chr->read(addr & 0x1FFF);
}

void Cart::write_ppu(Word addr, Byte val) {
	// mapper->write_ppu(addr, val);
	chr->write(addr & 0x1FFF, val);
}

Byte Cart::peek_ppu(Word addr) {
	// return mapper->peek_ppu(addr);
	return chr->peek(addr & 0x1FFF);
}

void Cart::step() {
	// mapper->step();
}
