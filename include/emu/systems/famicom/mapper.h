#pragma once

#include "common/data.h"
#include <memory>

namespace SuperDendy::Emu::Famicom {
	class Mapper {
	public:
		Mapper() {};
		virtual ~Mapper() {};

		// Each of these returns a mapped address
		// CPU operations are mapped to PRG memory
		// PPU operations are mapped to CHR memory
		// By default, each will return the address that was passed in
		virtual Word read_cpu(Word addr) { return addr; }
		virtual Word write_cpu(Word addr, Byte val) { return addr;}
		virtual Word peek_cpu(Word addr) { return addr; }
		virtual Word read_ppu(Word addr) { return addr; }
		virtual Word write_ppu(Word addr, Byte val) { return addr; }
		virtual Word peek_ppu(Word addr) { return addr;}
		virtual void step() = 0;

		static std::unique_ptr<Mapper> by_number(int num);
	};
}
