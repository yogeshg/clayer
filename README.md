# Clayer
Logging and analysis facility in C++ using compile time template meta programming.

## Simple Usage
Include the following headers in your project that needs logging.

```cpp
#include "logconfig.h"
#include "logger.h"
```

Clayer uses the standard C++ syntax to stream logging messages to the logger. The following line shows a sample usage of the logger. It prints the log message "Start program, begin logging" to `std::clog`, with logging level `DEBUG`

```cpp
LOG(DEBUG) << "Start program, begin logging";
```

To compile your program loaded with Clayer, issue the following command. Remember to put Clayer header files in the same directory

```g++ --std=c++1z -fconcepts -lpthread your_program.cpp```

## Further Information
Library design report, tutorial and documentation can be found in [Project Wiki].

## Authors
Yogesh Garg, Steven Shao, Andy Xu

[Project Wiki]: https://github.com/yogeshg/clayer/wiki/
