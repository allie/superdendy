#pragma once

#include "common.h"

namespace SuperDendy::Emu::Famicom {
	typedef union {
		struct {
			Byte right: 1;
			Byte left: 1;
			Byte down: 1;
			Byte up: 1;
			Byte start: 1;
			Byte select: 1;
			Byte b: 1;
			Byte a: 1;
		};
		Byte value;
	} ControllerState;

	class Controller {
	private:
		ControllerState controllers[2];
		Byte shift[2];

	public:
		Controller();

		Byte read(int num);
		void write(int num);
	};
}
