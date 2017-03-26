#include "logger.h"

int main() {
	logger::Logger* l;
	l = logger::Logger::getInstance();
	l->log() << "Logging 1\n";

	logger::Logger* l2 = logger::Logger::getInstance();
	l2->log() << "logging 2\n";
    return 0;
}

