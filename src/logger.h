#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>
#include <iostream>
#include <type_traits>
// TODO: how do you specify defaults? add a logconfig.h to the library

namespace logger {
	class Logger {
	private:
		Logger() {};
	public:
		static Logger &getInstance() {
            static Logger instance;
            return instance;
        }

        template <unsigned int N, typename std::enable_if <N >= THRESHOLD> :: type* = nullptr>
        std::ostream& log() {
            return std::cerr << "LOG ";
        }

        template <unsigned int N, typename std::enable_if <N < THRESHOLD> :: type* = nullptr>
        std::ostream& log() {
            // TODO: need to return a dev/null like stream
            return std::cerr << "SHOULDNT ACTUALLY LOG ";
        }

		~Logger() {

		}
	};

}

#endif /*__LOGGER_H__*/