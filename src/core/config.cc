#include "core/config.h"
#include "INIReader.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace SuperDendy::Core;

Config::Config(
	Logger& logger,
	const std::string& file
) :
	logger(logger),
	file(file)
{
	std::ifstream ifs(file);

	if (ifs.good()) {
		load();
	} else {
		logger.info("Could not locate configuration file, creating a new one");
		load_defaults();
		write();
	}
}

Config::~Config() {
	// Nothing for now
}

void Config::load() {
	INIReader reader(file);

	if (reader.ParseError() < 0) {
		logger.fatal("Failed to open configuration file");
		throw std::runtime_error("Failed to open configuration file");
	}

	window_pos.x = reader.GetInteger("interface", "window_pos_x", 100);
	window_pos.y = reader.GetInteger("interface", "window_pos_y", 100);
	window_scale = reader.GetInteger("interface", "window_scale", 1);
	volume = (float)reader.GetReal("interface", "volume", 1.0);

	logger.info("Successfully loaded configuration file");
}

void Config::load_defaults() {
	window_pos.x = 100;
	window_pos.y = 100;
	window_scale = 1;
	volume = 1.0f;
}

void Config::write() {
	std::ofstream ofs;
	ofs.open(file);

	ofs << "[interface]" << std::endl;
	ofs << "window_pos_x=" << window_pos.x << std::endl;
	ofs << "window_pos_y=" << window_pos.y << std::endl;
	ofs << "window_scale=" << window_scale << std::endl;
	ofs << "volume=" << volume << std::endl;

	ofs.close();
}
