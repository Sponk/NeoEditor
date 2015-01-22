# Shiny Profiler
# Advanced sample GNU Makefile
# 
# You have to build libShiny.a first in 'Shiny' directory.

TARGET = Advanced

CXXFLAGS = -g -Wall
INCLUDES = -I../../Shiny/include
LFLAGS = -L../../Shiny/lib/
LIBS = -lShiny

SRCS = $(wildcard *.cpp)


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LFLAGS) $(LIBS)

clean:
	$(RM) $(TARGET) $(OBJS)

