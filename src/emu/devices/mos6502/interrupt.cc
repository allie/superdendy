#include "emu/devices/mos6502/interrupt.h"
#include <iostream>
using namespace SuperDendy::Emu::MOS6502;

InterruptLine::InterruptLine() {
	// Set everything to false
	for (int i = 0; i < 4; i++) {
		status[i] = false;
	}
}

// Get the highest priority active interrupt request
int InterruptLine::get_current() {
	if (status[RESET]) {
		return RESET;
	} else if (status[NMI]) {
		return NMI;
	} else if (status[IRQ]) {
		return IRQ;
	}
	return NONE;
}

// Request an interrupt
void InterruptLine::request(int interrupt) {
	if (interrupt == NONE) {
		return;
	}
	status[interrupt] = true;
}

// Service an interrupt
void InterruptLine::service(int interrupt) {
	if (interrupt == NONE) {
		return;
	}
	status[interrupt] = false;
}
