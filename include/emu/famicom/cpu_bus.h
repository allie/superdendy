#pragma once

#include "common.h"
#include "emu/memory.h"
#include "core/logger.h"

namespace SuperDendy::Emu::Famicom {
	class CPUBus : public IMemory8 {
	private:
		SuperDendy::Core::Logger& logger;
		SimpleRAM8 ram;

	public:
		CPUBus(SuperDendy::Core::Logger& logger);
		~CPUBus();

		Byte peek(Word addr) const override;
		Byte read(Word addr) override;
		void write(Word addr, Byte val) override;
		int get_size() override;
	};
}
