# LuaDist CMake utility library.
# Provides sane project defaults and macros common to LuaDist CMake builds.
# 
# Copyright (C) 2007-2012 LuaDist.
# by David Manura, Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

## Extract information from dist.info
if ( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dist.info )
  message ( FATAL_ERROR
    "Missing dist.info file (${CMAKE_CURRENT_SOURCE_DIR}/dist.info)." )
endif ()
file ( READ ${CMAKE_CURRENT_SOURCE_DIR}/dist.info DIST_INFO )
if ( "${DIST_INFO}" STREQUAL "" )
  message ( FATAL_ERROR "Failed to load dist.info." )
endif ()
# Reads field `name` from dist.info string `DIST_INFO` into variable `var`.
macro ( _parse_dist_field name var )
  string ( REGEX REPLACE ".*${name}[ \t]?=[ \t]?[\"']([^\"']+)[\"'].*" "\\1"
           ${var} "${DIST_INFO}" )
  if ( ${var} STREQUAL DIST_INFO )
    message ( FATAL_ERROR "Failed to extract \"${var}\" from dist.info" )
  endif ()
endmacro ()
#
_parse_dist_field ( name DIST_NAME )
_parse_dist_field ( version DIST_VERSION )
_parse_dist_field ( license DIST_LICENSE )
_parse_dist_field ( author DIST_AUTHOR )
_parse_dist_field ( maintainer DIST_MAINTAINER )
_parse_dist_field ( url DIST_URL )
_parse_dist_field ( desc DIST_DESC )
message ( "DIST_NAME: ${DIST_NAME}")
message ( "DIST_VERSION: ${DIST_VERSION}")
message ( "DIST_LICENSE: ${DIST_LICENSE}")
message ( "DIST_AUTHOR: ${DIST_AUTHOR}")
message ( "DIST_MAINTAINER: ${DIST_MAINTAINER}")
message ( "DIST_URL: ${DIST_URL}")
message ( "DIST_DESC: ${DIST_DESC}")
string ( REGEX REPLACE ".*depends[ \t]?=[ \t]?[\"']([^\"']+)[\"'].*" "\\1"
         DIST_DEPENDS ${DIST_INFO} )
if ( DIST_DEPENDS STREQUAL DIST_INFO )
  set ( DIST_DEPENDS "" )
endif ()
message ( "DIST_DEPENDS: ${DIST_DEPENDS}")
## 2DO: Parse DIST_DEPENDS and try to install Dependencies with automatically using externalproject_add


## INSTALL DEFAULTS (Relative to CMAKE_INSTALL_PREFIX)
# Primary paths
set ( INSTALL_BIN bin CACHE PATH "Where to install binaries to." )
set ( INSTALL_LIB lib CACHE PATH "Where to install libraries to." )
set ( INSTALL_INC include CACHE PATH "Where to install headers to." )
set ( INSTALL_ETC etc CACHE PATH "Where to store configuration files" )
set ( INSTALL_SHARE share CACHE PATH "Directory for shared data." )

# Secondary paths
option ( INSTALL_VERSION
      "Install runtime libraries and executables with version information." OFF)
set ( INSTALL_DATA ${INSTALL_SHARE}/${DIST_NAME} CACHE PATH
      "Directory the package can store documentation, tests or other data in.")  
set ( INSTALL_DOC  ${INSTALL_DATA}/doc CACHE PATH
      "Recommended directory to install documentation into.")
set ( INSTALL_EXAMPLE ${INSTALL_DATA}/example CACHE PATH
      "Recommended directory to install examples into.")
set ( INSTALL_TEST ${INSTALL_DATA}/test CACHE PATH
      "Recommended directory to install tests into.")
set ( INSTALL_FOO  ${INSTALL_DATA}/etc CACHE PATH
      "Where to install additional files")

# Tweaks and other defaults
# Setting CMAKE to use loose block and search for find modules in source directory
set ( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )
set ( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )
option ( BUILD_SHARED_LIBS "Build shared libraries" ON )

# In MSVC, prevent warnings that can occur when using standard libraries.
if ( MSVC )
  add_definitions ( -D_CRT_SECURE_NO_WARNINGS )
endif ()

# RPath and relative linking
option ( USE_RPATH "Use relative linking." ON)
if ( USE_RPATH )
  string ( REGEX REPLACE "[^!/]+" ".." UP_DIR ${INSTALL_BIN} )
  set ( CMAKE_SKIP_BUILD_RPATH FALSE CACHE STRING "" FORCE )
  set ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE CACHE STRING "" FORCE )
  set ( CMAKE_INSTALL_RPATH $ORIGIN/${UP_DIR}/${INSTALL_LIB}
        CACHE STRING "" FORCE )
  set ( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE STRING "" FORCE )
  set ( CMAKE_INSTALL_NAME_DIR @executable_path/${UP_DIR}/${INSTALL_LIB}
        CACHE STRING "" FORCE )
endif ()

## MACROS
# Parser macro
macro ( parse_arguments prefix arg_names option_names)
  set ( DEFAULT_ARGS )
  foreach ( arg_name ${arg_names} )
    set ( ${prefix}_${arg_name} )
  endforeach ()
  foreach ( option ${option_names} )
    set ( ${prefix}_${option} FALSE )
  endforeach ()

  set ( current_arg_name DEFAULT_ARGS )
  set ( current_arg_list )
  foreach ( arg ${ARGN} )            
    set ( larg_names ${arg_names} )    
    list ( FIND larg_names "${arg}" is_arg_name )                   
    if ( is_arg_name GREATER -1 )
      set ( ${prefix}_${current_arg_name} ${current_arg_list} )
      set ( current_arg_name ${arg} )
      set ( current_arg_list )
    else ()
      set ( loption_names ${option_names} )    
      list ( FIND loption_names "${arg}" is_option )            
      if ( is_option GREATER -1 )
        set ( ${prefix}_${arg} TRUE )
      else ()
        set ( current_arg_list ${current_arg_list} ${arg} )
      endif ()
    endif ()
  endforeach ()
  set ( ${prefix}_${current_arg_name} ${current_arg_list} )
endmacro ()

