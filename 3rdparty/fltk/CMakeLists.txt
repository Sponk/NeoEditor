#
# "$Id: CMakeLists.txt 10368 2014-10-09 16:03:56Z AlbrechtS $"
#
# Main CMakeLists.txt to build the FLTK project using CMake (www.cmake.org)
# Written by Michael Surette
#
# Copyright 1998-2014 by Bill Spitzak and others.
#
# This library is free software. Distribution and use rights are outlined in
# the file "COPYING" which should have been included with this file.  If this
# file is missing or damaged, see the license at:
#
#     http://www.fltk.org/COPYING.php
#
# Please report all bugs and problems on the following page:
#
#     http://www.fltk.org/str.php
#

# Prevent annoying warning under Cygwin; this must be before project().
# Remove when CMake >= 2.8.4 is required
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project(FLTK)
cmake_minimum_required(VERSION 2.6)

#######################################################################
# define some macros
#######################################################################
include(CMake/macros.cmake)

#######################################################################
# basic setup
#######################################################################
include(CMake/setup.cmake)

#######################################################################
# check for headers, libraries and functions
#######################################################################
include(CMake/resources.cmake)

#######################################################################
# options
#######################################################################
include(CMake/options.cmake)

#######################################################################
# final config and export
#######################################################################
include(CMake/export.cmake)

#######################################################################
# build examples - these have to be after fluid is built/imported
#######################################################################
#if(OPTION_BUILD_EXAMPLES)
#   add_subdirectory(test)
#endif(OPTION_BUILD_EXAMPLES)

#######################################################################
# installation
#######################################################################
# include(CMake/install.cmake)
