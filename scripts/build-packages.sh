#!/bin/sh -e

rm -rf install/*

cmake $1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DNO_TESTS=TRUE
make -j7 package

if [ ! -d "build_emscripten" ]; then
	mkdir build_emscripten
fi

cd build_emscripten
# emconfigure cmake ../$1 -DNO_DOCUMENTATION=TRUE -DEMSCRIPTEN=TRUE -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -DSTANDARD_LUA=TRUE
emcmake cmake ../$1 -DNO_DOCUMENTATION=TRUE -DEMSCRIPTEN=TRUE -DCMAKE_BUILD_TYPE=Release -DSTANDARD_LUA=TRUE -DCMAKE_INSTALL_PREFIX=/usr
make -j7 package

## Building NeoWeb
#export ARCH_INSTALL=install/Neo/Arch

#if [ ! -d "../$ARCH_INSTALL/Web" ]; then
#	mkdir -p ../$ARCH_INSTALL/Web
#fi

#cp BinaryOutput/NeoWeb.html ../$ARCH_INSTALL/Web
#cp BinaryOutput/NeoWeb.js ../$ARCH_INSTALL/Web
#cp BinaryOutput/NeoWeb.html.mem ../$ARCH_INSTALL/Web
cd ..

## Crosscompiling with MinGW
if [ ! -d "build_mingw" ]; then
	mkdir build_mingw
fi

if [ ! -d "$ARCH_INSTALL/Win32" ]; then
	mkdir -p ../$ARCH_INSTALL/Win32
fi

cd build_mingw
cmake ../$1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DSTANDARD_LUA=TRUE -DCMAKE_TOOLCHAIN_FILE=../$1/CMake/Toolchains/CrossMinGW-Debian.cmake -DNO_TESTS=TRUE -DNO_DOCUMENTATION=TRUE -DNO_TOOLS=TRUE

make -j7 package
#rm ../$ARCH_INSTALL/Win32/Neo/*.a
cd ..

