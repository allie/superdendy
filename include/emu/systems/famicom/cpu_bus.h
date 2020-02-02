#pragma once

#include "common/data.h"
#include "emu/memory.h"
#include "emu/systems/famicom/cart.h"

namespace SuperDendy::Emu::Famicom {
	class CPUBus : public Memory8 {
	private:
		SimpleRAM8 ram;
		Cart& cart;
		std::string test_output;

	public:
		CPUBus(Cart& cart);
		~CPUBus();

		Byte peek(Word addr) const override;
		Byte read(Word addr) override;
		void write(Word addr, Byte val) override;
		int get_size() override;
	};
}
