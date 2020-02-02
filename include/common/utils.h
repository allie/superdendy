#pragma once

#include <vector>
#include <time.h>

namespace SuperDendy {
	// Global variable used to exit the main loop of each thread
	extern bool running;

	// Shortcut to check whether an value falls within a range (inclusive)
	inline bool in_range(int val, int min, int max) {
		return val >= min && val <= max;
	}

	// Helper functions
	timespec clock_diff(timespec start, timespec end);
};
