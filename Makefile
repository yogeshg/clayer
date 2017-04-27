# http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/

CC=g++ -g --std=c++1z -fconcepts -lpthread
CXX=g++ -g --std=c++1z -fconcepts -lpthread
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
TARGETS := bin/runner bin/analyser_test bin/tests
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall
# LIB := -pthread -lmongoclient -L lib -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
INC := -I include

default: $(TARGETS)

all:clean default

$(TARGETDIR)/%:$(BUILDDIR)/%.o
	@echo " Linking..."
	@echo " $(CC) $^ -o $@ $(LIB)"; $(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo " Compiling..."
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGETS)"; $(RM) -r $(BUILDDIR) $(TARGETS)

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

valgrind : $(TARGETS)
	valgrind -v --num-callers=20 --leak-check=yes --leak-resolution=high --show-reachable=yes $<

.PHONY: clean

.PHONY: all

.PHONY: default
