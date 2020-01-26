#pragma once

#include "common.h"

namespace SuperDendy::Emu::Famicom {
	class Memory {
	public:
		Memory() {};
		virtual ~Memory() {};

		virtual Byte read(Word addr) = 0;
		virtual void write(Word addr, Byte val) = 0;
		virtual Byte peek(Word addr) = 0;
		virtual int get_size();
	};
}
