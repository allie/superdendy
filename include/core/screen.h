#pragma once

#include "common/data.h"
#include <string>

namespace SuperDendy::Emu {
	class Screen {
		Screen();
		virtual ~Screen() = 0;
		virtual void update() = 0;
		virtual void render() = 0;
	};
}
