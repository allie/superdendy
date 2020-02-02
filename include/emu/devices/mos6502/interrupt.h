#pragma once

#include "emu/interrupt.h"

namespace SuperDendy::Emu::MOS6502 {
	enum Interrupt {
		NONE = 0,
		RESET,
		NMI,
		IRQ
	};

	class InterruptLine : public Emu::InterruptLine {
	private:
		bool status[4];

	public:
		InterruptLine();

		int get_current() override;
		void request(int interrupt) override;
		void service(int interrupt) override;
	};
}
