#include "emu/famicom/controller.h"

using namespace SuperDendy;
using namespace SuperDendy::Emu::Famicom;

Controller::Controller() {
	controllers[0].value = 0;
	controllers[1].value = 0;
	shift[0] = 0;
	shift[1] = 0;
}

Byte Controller::read(int num) {
	Byte val = (shift[num] & 0x80) > 0;
	shift[num] <<= 1;
	return val;
}

void Controller::write(int num) {
	shift[num] = controllers[num].value;
}
