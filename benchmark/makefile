CXX=g++
CXXSTD=--std=c++14
CXXOPTIMFLAGS=-O3 -g
CXXDEBUGFLAGS=-g -O0 -rdynamic
CXXFLAGS=-Wall $(CXXOPTIMFLAGS)
LIBS=

# find source files
SOURCEDIR := $(shell pwd)
BUILDDIR := $(shell pwd)
SOURCES := $(shell find $(SOURCEDIR) -maxdepth 1 -name '*.cpp')
OBJECTS := $(addprefix $(BUILDDIR)/,$(notdir $(SOURCES:.cpp=.o)))

# link
run_benchmark.x: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o run_benchmark.x $(LIBS)

# pull dependencies for existing .o files
-include $(OBJECTS:.o=.d)

# compile and generate dependency info
%.o: %.cpp %.d
	$(CXX) $(CXXSTD) -c $(CXXFLAGS) $*.cpp -o $*.o
	$(CXX) $(CXXSTD) -MM $(CXXFLAGS) $*.cpp > $*.d

# empty rule for dependency files
%.d: ;

clean:
	rm -f *core *.o *.d *.x
