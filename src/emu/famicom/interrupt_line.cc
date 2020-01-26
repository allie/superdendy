#include "emu/famicom/interrupt_line.h"

using namespace SuperDendy::Emu::Famicom;

InterruptLine::InterruptLine() {
	// Set everything to false
	for (int i = 0; i < 4; i++) {
		status[i] = false;
	}
}

// Get the highest priority active interrupt request
Interrupt InterruptLine::get_current() {
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
void InterruptLine::request(Interrupt interrupt) {
	if (interrupt == NONE) {
		return;
	}
	status[interrupt] = true;
}

// Service an interrupt
void InterruptLine::service(Interrupt interrupt) {
	if (interrupt == NONE) {
		return;
	}
	status[interrupt] = false;
}
