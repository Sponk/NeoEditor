#!/bin/bash -e

SCRIPT="$(readlink --canonicalize-existing "$0")"
SCRIPTPATH="$(dirname "$SCRIPT")"

echo Building in $1

mkdir -p build_win32
cd build_win32
bash -e $SCRIPTPATH/CPackWin32/build_unix.sh ../$1
cd ..

mkdir -p build_linux
cd build_linux
bash -e $SCRIPTPATH/CPackLinux/build_unix.sh ../$1
cd ..
