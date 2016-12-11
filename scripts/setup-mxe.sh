#!/bin/sh -e

cd ~
git clone https://github.com/mxe/mxe.git

cd mxe
git pull

cat << EOF >> settings.mk

# This is a template of configuration file for MXE. See
# docs/index.html for more extensive documentations.

# This variable controls the number of compilation processes
# within one package ("intra-package parallelism").
# JOBS := 4

# This variable controls where intermediate files are created
# this is necessary when compiling inside a virtualbox shared
# directory. Some commands like strip fail in there with Protocol error
# default is the current directory
MXE_TMP := /tmp

# This variable controls the targets that will build.
MXE_TARGETS :=  i686-w64-mingw32.shared.unix.dwarf
# i686-w64-mingw32.shared x86_64-w64-mingw32.shared
# x86_64-w64-mingw32.static i686-w64-mingw32.static

# This variable controls which plugins are in use.
# See plugins/README.md for further information.
#override MXE_PLUGIN_DIRS += plugins/apps plugins/native

# This variable controls the download mirror for SourceForge,
# when it is used. Enabling the value below means auto.
#SOURCEFORGE_MIRROR := downloads.sourceforge.net

# The three lines below makes `make` build these "local
# packages" instead of all packages.
#LOCAL_PKG_LIST := boost curl file flac lzo pthreads vorbis wxwidgets
#.DEFAULT local-pkg-list:
#local-pkg-list: $(LOCAL_PKG_LIST)

MXE_GCC_THREADS := posix

EOF

make -j2 gcc pthreads qt5 nsis

## Fix pthread library so it can be found
ln i686-w64-mingw32.shared.unix.dwarf/lib/libpthread.dll.a i686-w64-mingw32.shared.unix.dwarf/lib/libpthread.a

ln usr/bin/i686-w64-mingw32.shared.unix.dwarf-makensis usr/bin/makensis
