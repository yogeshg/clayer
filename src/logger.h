#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <memory>
#include <iostream>
#include <type_traits>

namespace logger {
    constexpr int THRESHOLD = 30;
	class Logger {
	private:
		Logger() {};
	public:
		static Logger &getInstance() {
            static Logger instance;
            return instance;
        }
        std::ostream& log() {
            return std::cerr << "LOG ";
        }

        template <unsigned int N, typename std::enable_if <N >= THRESHOLD> :: type* = nullptr>
        std::ostream& log2() {
            return std::cerr << "LOG ";
        }

        template <unsigned int N, typename std::enable_if <N < THRESHOLD> :: type* = nullptr>
        std::ostream& log2() {
            return std::cerr << "SHOULDNT ACTUALLY LOG ";
        }

		~Logger() {

		}
	};

}

#endif /*__LOGGER_H__*/