## Contains a small CMake project that merely builds Linux packages
## from prebuilt binaries so multiple compilation passes
## produce just one installer
##
## This script requires a few variables to be set:
##
## MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, COMMIT_VERSION -> Version description
## PACKAGE_ROOT -> The root directory of the binary files to build the package from
##

project(NeoLinux)
cmake_minimum_required(VERSION 2.8)

## Packaging
set(CPACK_PACKAGE_NAME "Neo")

set(CPACK_PACKAGE_VENDOR "Yannick Pflanzer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neo game engine and editor.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/../../README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../../LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/../../README.md")
set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")

## Produce packages for several Linux distros
SET(CPACK_PACKAGE_CONTACT Yannick Pflanzer <www.neo-engine.de>)
SET(CPACK_GENERATOR "STGZ;TGZ;DEB;RPM;")
SET(CPACK_STRIP_FILES ON)

install(DIRECTORY ${PACKAGE_ROOT}
  DESTINATION ./
  COMPONENT engine
  PATTERN */Arch/* EXCLUDE)

install(DIRECTORY ${PACKAGE_ROOT}/../share
  DESTINATION ./
  COMPONENT engine)

install(DIRECTORY ${PACKAGE_ROOT}/Arch/Win32
  DESTINATION ./Neo/Arch
  COMPONENT win32_support)

install(DIRECTORY ${PACKAGE_ROOT}/Arch/Web
  DESTINATION ./Neo/Arch
  COMPONENT web_support)

set(CPACK_COMPONENT_ENGINE_DISPLAY_NAME "Core Engine")
set(CPACK_COMPONENT_WIN32_SUPPORT_DISPLAY_NAME "Win32 Support")
set(CPACK_COMPONENT_WEB_SUPPORT_DISPLAY_NAME "Web Support")

include(CPack)

