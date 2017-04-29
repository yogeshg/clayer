all: boost glog

boost: boost.cpp
	g++ -O3 -DBOOST_LOG_DYN_LINK boost.cpp -lboost_log -lboost_thread -lboost_system -lpthread -o boost

glog: glog.cpp
	g++ --std=c++1z -fconcepts -lpthread -O3 glog.cpp -lglog -o glog
