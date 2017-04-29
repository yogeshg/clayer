#include <thread>
#include <vector>
#include <utility>
#include <cstdlib>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
const int iterations = 1000*50;
const int thread_count = 1;
void worker() {
  for (int i = 0; i < iterations; i++) {
    BOOST_LOG_TRIVIAL(debug) << "DEBUG message";
    BOOST_LOG_TRIVIAL(info) << "INFO message";
    BOOST_LOG_TRIVIAL(warning) << "WARNING message";
    BOOST_LOG_TRIVIAL(error) << "ERROR message";
    BOOST_LOG_TRIVIAL(trace) << "TRACE message";
  }
}
namespace expr = boost::log::expressions;
int main(int, char*[])
{
  using namespace boost::log;
  /*boost::log::add_file_log(
      keywords::file_name = "boost.log",
      keywords::format = (expr::stream << expr::attr<unsigned int>("LineID")
                                       << ": <" << trivial::severity
                                       << "> " << expr::smessage));*/

  BOOST_LOG_TRIVIAL(info) << "Begin the performance test";
  std::vector<std::thread> worker_threads;
  for(int i = 0; i < thread_count; i++){
    worker_threads.push_back(std::move(std::thread(worker)));
  }

  for(auto &thread : worker_threads){
    thread.join();
  }

    return 0;
}
