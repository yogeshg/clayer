#include "logger.h"

int main() {

	logger::Logger* l;
	l = logger::Logger::getInstance();
	l->log() << "Logging 1\n";

	logger::Logger::getInstance()->log() << "logging 2\n";
	logger::Logger2<100>::getInstance()->log() << "logging 3\n";


    return 0;
}

