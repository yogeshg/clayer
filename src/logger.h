#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>
#include <iostream>

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
}

#endif /*__LOGGER_H__*/