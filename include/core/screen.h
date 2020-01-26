#pragma once

#include "common.h"
#include <string>

namespace SuperDendy::Emu {
	class IScreen {
		IScreen();
		virtual ~IScreen() = 0;
		virtual void update() = 0;
		virtual void render() = 0;
	};
}
