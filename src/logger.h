#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>
#include <iostream>
#include <type_traits>

namespace logger {
	class Logger {
	private:
		static bool is_instantiated;
		static Logger* instance;
		static int id;
		Logger() {id+=1;};
	public:
		static Logger* getInstance();
		std::ostream& log();
		~Logger() {
			is_instantiated = false;
		}
	};

	template <unsigned int N, typename std::enable_if <N >= 100> :: type* = nullptr>
	class Logger2 {
		static bool is_instantiated;
		static Logger2* instance;
		static int id;
		static Logger* logger;
	private:
		Logger2() {
			id+=1;
			logger = Logger::getInstance();
		};
	public:
		static Logger2* getInstance() {
			if(!is_instantiated) {
				instance = new Logger2();
				is_instantiated = true;
			}
			return instance;
		}

		std::ostream& log() {
			return logger->log();
		}
	};

	// template <unsigned int N, typename std::enable_if <N < 100> :: type* = nullptr>
	// class Logger2 {
	// 	static bool is_instantiated;
	// 	static Logger2* instance;
	// 	static int id;
	// 	static Logger* logger;
	// 	Logger2() {
	// 		id+=1;
	// 		logger = Logger::getInstance();
	// 	};
	// public:
	// 	static Logger2* getInstance() {
	// 		if(!is_instantiated) {
	// 			instance = new Logger2();
	// 			is_instantiated = true;
	// 		}
	// 		return instance;
	// 	}

	// 	std::ostream& log() {
	// 		return std::cerr;
	// 	}
	// };

}

#endif /*__LOGGER_H__*/