#!/bin/sh -e

OUTPATH=$PWD/Neo

SCRIPT="$(readlink --canonicalize-existing "$0")"
SCRIPTPATH="$(dirname "$SCRIPT")"

echo Building Linux version

mkdir -p build_linux
cd build_linux

## Build using the native Linux compiler
## This build includes documentation, tools and tests
cmake ../$1 -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_INSTALL_PREFIX=$OUTPATH/../ # &>> log.txt

make -j8 install # &>> log.txt
cd ..

echo Building Win32 version

mkdir -p build_win32
cd build_win32

## Build using the MXE crosscompiler
## Requires the MXE bin directory to be in PATH!
## This build will only build the player and the libraries needed
## to publish a game.
i686-w64-mingw32.shared.unix.dwarf-cmake ../$1 -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_INSTALL_PREFIX=$OUTPATH/Arch/Win32 \
	  -DSTANDARD_LUA=TRUE \
	  -DNO_TESTS=TRUE \
	  -DNO_DOCUMENTATION=TRUE \
	  -DNO_TOOLS=TRUE # &>> log.txt

make -j8 install # &>> log.txt

cd ..

mkdir -p build_emscripten
cd build_emscripten

## Building NeoWeb
## Only builds modules required to publish a game to the web

echo Building NeoWeb version

emcmake cmake ../$1 -DNO_DOCUMENTATION=TRUE \
		-DEMSCRIPTEN=TRUE \
		-DCMAKE_BUILD_TYPE=Release \
		-DSTANDARD_LUA=TRUE # &>> log.txt

make -j8 # &>> log.txt

WEB_INSTALL=$OUTPATH/Arch/Web
mkdir -p $WEB_INSTALL

cp BinaryOutput/NeoWeb.html $WEB_INSTALL
cp BinaryOutput/NeoWeb.js $WEB_INSTALL
cp BinaryOutput/NeoWeb.html.mem $WEB_INSTALL
cd ..

## Workaround for add_swig_module ignoring the library prefix
if [ -f Neo/libNeoEditor.so ] && [ ! -f Neo/NeoEditor.so ]; then
    echo "Fixing libNeoEditor.so issue"
	ln Neo/libNeoEditor.so Neo/NeoEditor.so
fi

echo Building packages with CMakeLists.txt from $SCRIPTPATH
cmake $SCRIPTPATH \
	  -DMAJOR_VERSION=0 \
	  -DMINOR_VERSION=6 \
	  -DPATCH_VERSION=0 \
	  -DPACKAGE_ROOT=$OUTPATH # &>> log.txt

cpack -G DEB # package # &>> log.txt
fakeroot cpack -G TGZ
cpack -G RPM
fakeroot cpack -G STGZ

echo "Finished."

