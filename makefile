CXX=g++
CXXSTD=--std=c++14
CXXFLAGS=-Wall
CXXOPTIMFLAGS=-O3
CXXDEBUGFLAGS=-g -O0
CXXFLAGS=$(CXXDEBUGFLAGS)
LIBS=

# find source files
SOURCEDIR := $(shell pwd)
BUILDDIR := $(shell pwd)
SOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')
OBJECTS := $(addprefix $(BUILDDIR)/,$(notdir $(SOURCES:.cpp=.o)))

# link
main.x: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o main.x $(LIBS)

# pull dependencies for existing .o files
-include $(OBJECTS:.o=.d)

# compile and generate dependency info
%.o: %.cpp
	$(CXX) $(CXXSTD) -c $(CXXFLAGS) $*.cpp -o $*.o
	$(CXX) $(CXXSTD) -MM $(CXXFLAGS) $*.cpp > $*.d

clean:
	rm -f *core *.o *.d *.x
