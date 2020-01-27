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
		Logger() {};

		static std::ofstream ofs;
		static void write_line(const std::string& prefix, const std::string& message);

	public:
		static LogLevel level;

		~Logger();

		static void start(const std::string& file, LogLevel level);
		static void debug(const std::string& message);
		static void info(const std::string& message);
		static void warn(const std::string& message);
		static void error(const std::string& message);
		static void fatal(const std::string& message);
		static void stop();
	};
};
