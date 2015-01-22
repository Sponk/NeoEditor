# Shiny Profiler
# Lua lib GNU Makefile


STATIC_LIB = libShinyLua.a
SHARED_LIB = libShinyLua.so
LIB_PATH = lib

CXXFLAGS = -g -O3 -fPIC -Wall
INCLUDES = \
	-Iinclude \
	-I../Shiny/include \
	-I/usr/include/lua5.1

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)



all: staticlib sharedlib

# build and move static lib to 'lib' directory
staticlib: $(STATIC_LIB)
	mkdir -p $(LIB_PATH)
	mv $(STATIC_LIB) $(LIB_PATH)

# build and move shared lib to 'lib' directory
sharedlib: $(SHARED_LIB)
	mkdir -p $(LIB_PATH)
	mv $(SHARED_LIB) $(LIB_PATH)


# builds static lib
$(STATIC_LIB): $(OBJS)
	$(AR) rcs $@ $^

# builds shared lib
$(SHARED_LIB): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(INCLUDES) -o $@ $^

# builds the object files
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@


# removes all object files compilation results
clean:
	$(RM) $(STATIC_LIB) $(SHARED_LIB) $(OBJS)

help:
	@echo "Available targets for Shiny Lua lib"
	@echo " staticlib: Build static library libShinyLua.a"
	@echo " sharedlib: Build shared library libShinyLua.so"
	@echo ""
	@echo " clean: Clean up directories"

.PHONY: all staticlib sharedlib help clean
