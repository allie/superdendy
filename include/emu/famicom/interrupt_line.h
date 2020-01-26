#pragma once

#include "common.h"

namespace SuperDendy::Emu::Famicom {
	enum Interrupt {
		NONE = 0,
		RESET,
		NMI,
		IRQ
	};

	class InterruptLine {
	private:
		bool status[4];

	public:
		InterruptLine();

		Interrupt get_current();
		void request(Interrupt interrupt);
		void service(Interrupt interrupt);
	};
};
