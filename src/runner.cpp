#include "logger.h"

int main() {

    logger::Logger::getInstance().log() << "logging 1\n";
    logger::Logger::getInstance().log2<40>() << "logging 2\n";
    logger::Logger::getInstance().log2<20>() << "logging 3\n";
	// logger::Logger2<100>::getInstance()->log() << "logging 3\n";


    return 0;
}

