#pragma once

#include "common/data.h"

namespace SuperDendy::Emu {
	class InterruptLine {
	public:
		virtual int get_current() = 0;
		virtual void request(int interrupt) = 0;
		virtual void service(int interrupt) = 0;
	};
}
