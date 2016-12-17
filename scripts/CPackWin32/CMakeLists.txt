## Contains a small CMake project that merely builds a Win32 setup.exe
## from prebuilt binaries so multiple compilation passes
## produce just one installer
##
## This script requires a few variables to be set:
##
## MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, COMMIT_VERSION -> Version description
## PACKAGE_ROOT -> The root directory of the binary files to build the package from
##

project(NeoWin32)
cmake_minimum_required(VERSION 2.8)

# set(CMAKE_INSTALL_PREFIX "C:/Program Files/")

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

## NSIS does not like png's
set(CPACK_PACKAGE_ICON "")

set(CPACK_NSIS_MENU_LINKS "Neo/NeoPlayer2.exe" "Neo Editor"
  "Neo/ProjectManager.exe" "Neo Project Manager"
  "Neo/docs/SDK/html/index.html" "Neo Documentation")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

## Produce a plain archive and a setup installer
set(CPACK_GENERATOR "NSIS;ZIP")

install(DIRECTORY ${PACKAGE_ROOT}
  DESTINATION ./
  COMPONENT engine
  PATTERN */Arch/* EXCLUDE)

install(DIRECTORY ${PACKAGE_ROOT}/Arch/Linux
  DESTINATION ./Neo/Arch
  COMPONENT linux_support)

install(DIRECTORY ${PACKAGE_ROOT}/Arch/Web
  DESTINATION ./Neo/Arch
  COMPONENT web_support)

message( STATUS "    Installing system-libraries: MinGW DLLs.")
get_filename_component(MINGW_PATH ${CMAKE_CXX_COMPILER} PATH)
set(MINGW_PATH ${MINGW_PATH}/../i686-w64-mingw32.shared.unix.dwarf/bin/)
set(INSTALL_SYSTEM_RUNTIME_LIBS
  ${MINGW_PATH}/libwinpthread-1.dll
  ${MINGW_PATH}/libgcc_s_sjlj-1.dll
  ${MINGW_PATH}/libstdc++-6.dll
  ${MINGW_PATH}/libharfbuzz-0.dll
  ${MINGW_PATH}/libpng16-16.dll
  ${MINGW_PATH}/zlib1.dll
  ${MINGW_PATH}/libglib-2.0-0.dll
  ${MINGW_PATH}/libpcre16-0.dll
  ${MINGW_PATH}/libpcre-1.dll
  ${MINGW_PATH}/libfreetype-6.dll
  ${MINGW_PATH}/libbz2.dll
  ${MINGW_PATH}/libintl-8.dll
  ${MINGW_PATH}/libiconv-2.dll)

install(PROGRAMS ${INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ./Neo COMPONENT engine)

set(CPACK_COMPONENT_ENGINE_DISPLAY_NAME "Core Engine")
set(CPACK_COMPONENT_LINUX_SUPPORT_DISPLAY_NAME "Linux Support")
set(CPACK_COMPONENT_WEB_SUPPORT_DISPLAY_NAME "Web Support")

include(CPack)

