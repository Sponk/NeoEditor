# not GPL, thus no COPYING
AUTOMAKE_OPTIONS=dist-bzip2 foreign

ACLOCAL_AMFLAGS=	-I m4

SUBDIRS=lib src regress

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libzip.pc

EXTRA_DIST=	LICENSE \
		CMakeLists.txt \
		cmake-config.h.in \
		cmake-zipconf.h.in \
		libzip.pc.in \
		libzip-uninstalled.pc.in \
		m4
