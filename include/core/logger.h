#pragma once

#include "common.h"
#include <string>
#include <fstream>

namespace SuperDendy::Core {
	enum LogLevel {
		OFF = 0,
		FATAL,
		ERROR,
		WARN,
		INFO,
		DEBUG,
		ALL
	};

	class Logger {
	private:
		std::ofstream ofs;
		void write_line(const std::string& prefix, const std::string& message);

	public:
		LogLevel level;

		Logger(const std::string& file, LogLevel level);
		~Logger();

		void debug(const std::string& message);
		void info(const std::string& message);
		void warn(const std::string& message);
		void error(const std::string& message);
		void fatal(const std::string& message);
	};
};
