#pragma once

#include "common.h"
#include "emu/famicom/mapper.h"
#include <string>
#include <memory>

namespace SuperDendy::Emu::Famicom {
	enum NametableMirroringMode {
		V_MIRROR = 0,
		H_MIRROR,
		Q_SCREEN
	};

	enum TVMode {
		NTSC = 0,
		PAL
	};

	class Cart {
	private:
		Byte prg_rom_size;
		Byte chr_rom_size;
		Byte prg_ram_size;
		Byte vram_arr;
		Byte has_sram;
		Byte has_trainer;
		Byte mapper_num;
		Byte vs_unisystem;
		Byte playchoice;
		Byte nes2;
		Byte tv_mode;
		Byte* trainer;
		Byte* prg;
		Byte* chr;
		Byte* sram;
		std::unique_ptr<Mapper> mapper;

	public:
		std::string name;

		Cart(const char* file);
		~Cart();

		Byte read_cpu(Word addr);
		void write_cpu(Word addr, Byte val);
		Byte peek_cpu(Word addr);
		Byte read_ppu(Word addr);
		void write_ppu(Word addr, Byte val);
		Byte peek_ppu(Word addr);
	};
}
