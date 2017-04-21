# http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/

CC=g++ -g --std=c++1z -fconcepts -lpthread
CXX=g++ -g --std=c++1z -fconcepts -lpthread

exe=t1

all : $(exe)
clean :
	rm -r *.o $(exe) *.dSYM

valgrind : $(exe)
	valgrind -v --num-callers=20 --leak-check=yes --leak-resolution=high --show-reachable=yes $<

.PHONY: clean

.PHONY: all
