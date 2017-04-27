# clayer
Logging and analysis facility in C++ using compile time template meta programming
[Project Wiki] (https://github.com/yogeshg/clayer/wiki)
## Aim
* Logging Library - full flexibility of logging including levels, formats, logging to different streams
* Analysis of Logs - millions of log records get created every day, not only should a good library create them
  it should also be able to analyze them
* Performance
    - Make compile decisions and compile time
    - zero overhead for level not in use
* Intuition
    - Easy syntax for setting up logs
    - Intuitive syntax for actual logging, users should be able to migrate from std::cerr using simple find and replace

## Scope of Project

### Version 0.8

### Version 1.0

### Version 1.2

## HTTP Server Tester
### Environment Setup
1. Install Boost Library and set the environment variable `BOOST_ROOT` to be the top level directory of the library.
2. Create a new directory `build` inside `clayer/test/server` and run `cmake ..`
3. Run `make`
4. The server is already loaded with `clayer` logging library. Run `./http_examples` to start an HTTP server on `127.0.0.1:8080`
5. Random test - simulating user behavior on the server. Run `./logtest.sh` inside `clayer/server/tests`
6. Stress test - TODO

## Links
* [Proposal] contains
* We will follow this [Project Structure]


[Proposal]: https://docs.google.com/document/d/1WWg79GEwaBX3Nd1sq--ZGRc9JdGWx78QBPt322kT4vc/edit#heading=h.a7w2y67mfsq4
[Project Structure]: http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/
