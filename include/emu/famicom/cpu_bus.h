#pragma once

#include "common.h"
#include "emu/memory.h"

namespace SuperDendy::Emu::Famicom {
	class CPUBus : public IMemory8 {
	private:
		SimpleRAM8 ram;

	public:
		CPUBus();
		~CPUBus();

		Byte peek(Word addr) const override;
		Byte read(Word addr) override;
		void write(Word addr, Byte val) override;
		int get_size() override;
	};
}
