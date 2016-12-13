#!/bin/sh -e

cd ~
git clone https://github.com/mxe/mxe.git

cd mxe
# git pull

echo Writing settings to file
cat << EOF > settings.mk
MXE_TMP := /tmp
MXE_TARGETS :=  i686-w64-mingw32.shared.unix.dwarf
MXE_GCC_THREADS := posix
EOF

echo Building MXE crosscompiler
make -j2 gcc pthreads qt5 nsis

## Fix pthread library so it can be found
echo Fixing some links
ln usr/i686-w64-mingw32.shared.unix.dwarf/lib/libpthread.dll.a usr/i686-w64-mingw32.shared.unix.dwarf/lib/libpthread.a

ln usr/bin/i686-w64-mingw32.shared.unix.dwarf-makensis usr/bin/makensis
