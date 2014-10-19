# Shiny Profiler
# MainLoop sample GNU Makefile
# 
# You have to build libShiny.a first in 'Shiny' directory.

TARGET = MainLoop
CXXFLAGS = -g -Wall
INCLUDES = -I../../Shiny/include
LFLAGS = -L../../Shiny/lib/
LIBS = -lShiny
SRCS = MainLoop.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LFLAGS) $(LIBS)

clean:
	$(RM) $(TARGET)

