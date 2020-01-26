#pragma once

#include "common.h"
#include <memory>

namespace SuperDendy::Emu::Famicom {
	class Mapper {
	public:
		Mapper() {};
		virtual ~Mapper() {};

		virtual Byte read_cpu(Word addr) = 0;
		virtual void write_cpu(Word addr, Byte val) = 0;
		virtual Byte peek_cpu(Word addr) = 0;
		virtual Byte read_ppu(Word addr) = 0;
		virtual void write_ppu(Word addr, Byte val) = 0;
		virtual Byte peek_ppu(Word addr) = 0;
		virtual void step();

		static std::unique_ptr<Mapper> by_number(int num);
	};
}
