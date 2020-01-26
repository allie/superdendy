#include "core/logger.h"
#include <iostream>
#include <iomanip>

using namespace SuperDendy::Core;

Logger::Logger(
	const std::string& file,
	LogLevel level
) :
	level(level)
{
	ofs.open(file, std::ios_base::app);
	write_line("LOGGER", "Beginning logging session");
}

Logger::~Logger() {
	write_line("LOGGER", "Ending logging session");
	ofs << std::endl;
	ofs.close();
}

void Logger::debug(const std::string& message) {
	if (level < DEBUG) {
		return;
	}
	write_line("DEBUG", message);
}

void Logger::info(const std::string& message) {
	if (level < INFO) {
		return;
	}
	write_line("INFO", message);
}

void Logger::warn(const std::string& message) {
	if (level < WARN) {
		return;
	}
	write_line("WARN", message);
}

void Logger::error(const std::string& message) {
	if (level < ERROR) {
		return;
	}
	write_line("ERROR", message);
}

void Logger::fatal(const std::string& message) {
	if (level < FATAL) {
		return;
	}
	write_line("FATAL", message);
}

void Logger::write_line(const std::string& prefix, const std::string& message) {
	// Write the timestamp first
	// [YYYY-MM-DD HH:MM:SS]
	std::time_t now = std::time(nullptr);
	ofs << std::put_time(std::localtime(&now), "[%Y-%m-%d %H:%M:%S] ");

	// Write the log level or other prefix
	// PREFIX:
	ofs << prefix << ": ";

	// Write the actual message plus a newline
	ofs << message << std::endl;
}
