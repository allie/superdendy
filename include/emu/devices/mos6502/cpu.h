#pragma once

#include "common/data.h"
#include "emu/memory.h"
#include "emu/devices/mos6502/interrupt.h"

namespace SuperDendy::Emu::MOS6502 {
	enum Mode {
		NES
	};

	enum {
		CLOCK_SPEED = 1789773
	};

	enum AddressingMode {
		IMP, // Implied
		ACC, // Accumulator
		IMM, // Immediate
		ZPG, // Zero-page
		ZPX, // Zero-page, X
		ZPY, // Zero-page, Y
		REL, // Relative
		ABS, // Absolute
		ABX, // Absolute, X
		ABY, // Absolute, Y
		IND, // Indirect
		IDX, // Indexed indirect
		IDY  // Indirect indexed
	};

	enum Instruction {
		ADC,
		AND,
		ASLA,
		ASL,
		BCC,
		BCS,
		BEQ,
		BIT,
		BMI,
		BNE,
		BPL,
		BRK,
		BVC,
		BVS,
		CLC,
		CLD,
		CLI,
		CLV,
		CMP,
		CPX,
		CPY,
		DEC,
		DEX,
		DEY,
		EOR,
		INC,
		INX,
		INY,
		JMP,
		JSR,
		LDA,
		LDX,
		LDY,
		LSRA,
		LSR,
		NOP,
		ORA,
		PHA,
		PHP,
		PLA,
		PLP,
		ROLA,
		ROL,
		RORA,
		ROR,
		RTI,
		RTS,
		SBC,
		SEC,
		SED,
		SEI,
		STA,
		STX,
		STY,
		TAX,
		TAY,
		TSX,
		TXA,
		TXS,
		TYA,
		DCP,
		ISB,
		LAX,
		RLA,
		RRA,
		SAX,
		SLO,
		SRE
	};

	class CPU {
	private:
		Memory8& memory;
		InterruptLine& interrupt_line;

		void pushb(Byte val);
		void pushw(Word val);
		Byte pullb();
		Word pullw();

	protected:
		Word pc;
		Byte sp;
		Byte a;
		Byte x;
		Byte y;
		Byte s; // status

		Byte opcode;
		Word oper_addr;
		Byte operand;
		Word ind_oper_addr;
		Byte ind_operand;
		Dword cycles;
		Dword suspended;

#ifdef NESTEST
		void nestest_disassemble1();
		void nestest_disassemble2(AddressingMode addr_mode);
		void nestest_disassemble3();
#endif

	public:
		CPU(
			Memory8& memory,
			InterruptLine& interrupt_line,
			Mode mode
		);
		~CPU();

		void power_on();
		void reset();
		void suspend(Dword duration);
		Dword step();
	};
};
