#!/usr/bin/env sh -e

echo Generating package in multiple passes to include all supported platforms.

export EMINSTALL_DIR=$PWD/Arch/Emscripten
export WIN32INSTALL_DIR=$PWD/Arch/Win32

mkdir _build
cd _build

mkdir build_emscripten
cd build_emscripten
emconfigure cmake ../../ -DNO_DOCUMENTATION=TRUE -DEMSCRIPTEN=TRUE -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -DSTANDARD_LUA=TRUE
make -j4

cp BinaryOutput/NeoWeb.html $EMINSTALL_DIR
cp BinaryOutput/NeoWeb.js $EMINSTALL_DIR
cp BinaryOutput/NeoWeb.html.mem $EMINSTALL_DIR
cp BinaryOutput/NeoWeb.data $EMINSTALL_DIR
cd ..

mkdir build_win32
cd build_win32
cmake ../NeoEditor -DCMAKE_BUILD_TYPE=Release -DNO_DOCUMENTATION=TRUE -DCMAKE_INSTALL_PREFIX=$WIN32INSTALL_DIR -DSTANDARD_LUA=TRUE -DCMAKE_TOOLCHAIN_FILE=../NeoEditor/CMake/Toolchains/CrossMinGW-Debian.cmake
make -j4
make install
cd ..

mkdir build_native
cd build_native
cmake ../../
make -j4
make package
cd ..

# rm -rf build_native