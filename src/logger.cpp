#include "logger.h"

using namespace logger;

bool Logger::is_instantiated = false;
int Logger::id = 0;

Logger* Logger::instance = nullptr;

Logger* Logger::getInstance() {
	if(!is_instantiated) {
		instance = new Logger();
		is_instantiated = true;
	}
	return instance;
}

std::ostream& Logger::log() {
	return std::cerr << "LOG" << id << " ";;
}
