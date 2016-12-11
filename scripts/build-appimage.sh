#!/bin/sh -e

rm -rf install/*

cmake $1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PWD/install -DNO_TESTS=TRUE
make -j7 install

if [ ! -d "build_emscripten" ]; then
	mkdir build_emscripten
fi

cd build_emscripten
# emconfigure cmake ../$1 -DNO_DOCUMENTATION=TRUE -DEMSCRIPTEN=TRUE -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release -DSTANDARD_LUA=TRUE
emcmake cmake ../$1 -DNO_DOCUMENTATION=TRUE -DEMSCRIPTEN=TRUE -DCMAKE_BUILD_TYPE=Release -DSTANDARD_LUA=TRUE
make -j7

## Building NeoWeb
export ARCH_INSTALL=install/Neo/Arch

if [ ! -d "../$ARCH_INSTALL/Web" ]; then
	mkdir -p ../$ARCH_INSTALL/Web
fi

cp BinaryOutput/NeoWeb.html ../$ARCH_INSTALL/Web
cp BinaryOutput/NeoWeb.js ../$ARCH_INSTALL/Web
cp BinaryOutput/NeoWeb.html.mem ../$ARCH_INSTALL/Web
cd ..

## Crosscompiling with MinGW
if [ ! -d "build_mingw" ]; then
	mkdir build_mingw
fi

if [ ! -d "$ARCH_INSTALL/Win32" ]; then
	mkdir -p ../$ARCH_INSTALL/Win32
fi

cd build_mingw
cmake ../$1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../$ARCH_INSTALL/Win32 -DSTANDARD_LUA=TRUE -DCMAKE_TOOLCHAIN_FILE=../$1/CMake/Toolchains/CrossMinGW-Debian.cmake -DNO_TESTS=TRUE -DNO_DOCUMENTATION=TRUE -DNO_TOOLS=TRUE

make -j7 install
rm ../$ARCH_INSTALL/Win32/Neo/*.a
cd ..

## Setting up config files
cat << EOF >> install/AppRun
#!/bin/sh
HERE=\$(dirname \$(readlink -f "\${0}"))
cd \$HERE/Neo
./ProjectManager
EOF

chmod +x install/AppRun
ln install/share/icons/neo-icon.png install/neo.png

cat << EOF >> install/neo.desktop
[Desktop Entry]
Name=Neo
Exec=neo
Icon=neo.png
EOF

## Building AppImage
rm -f Neo.AppImage
AppImageAssistant install Neo.AppImage
