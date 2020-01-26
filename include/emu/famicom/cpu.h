#pragma once

#include "common.h"

namespace SuperDendy::Emu::Famicom {
	enum {
		CLOCK_SPEED = 1789773
	};

	class CPU {
	private:
	protected:
		Word pc;
		Byte sp;
		Byte a;
		Byte x;
		Byte y;

		union {
			struct {
				Byte c : 1;
				Byte z : 1;
				Byte i : 1;
				Byte d : 1;
				Byte b : 2; // unused
				Byte v : 1;
				Byte n : 1;
			};
			Byte value;
		} status;

		Byte opcode;
		Word oper_addr;
		Byte operand;
		Word ind_oper_addr;
		Byte ind_operand;
		Dword cycles;
		Dword suspended;

	public:
		CPU();
		~CPU();

		void power_on();
		void reset();
		void suspend(Dword duration);
		Dword step();
	}
}
