## Find sources
FILE(GLOB_RECURSE SOURCES *.lua)
INSTALL(FILES ${SOURCES} DESTINATION ${NEO_INSTALL_DIR}/LuaApi)

INSTALL(DIRECTORY ${NEO_BINARY_OUTPUT}/../Documentation/LuaDoc DESTINATION ${NEO_INSTALL_DIR}/Documentation)

INCLUDE(CopyFiles)
copy_files("*.lua" "${NEO_BINARY_OUTPUT}/scripts/SDK" "copy-lua-api")

IF(NOT NO_DOCUMENTATION)
  ## TODO: Document behavior!
  ## TODO: Get proper Lua executable path!
ADD_CUSTOM_TARGET(docl ALL
  COMMAND ${LUA_EXECUTABLE} ${NEO_SOURCE_DIR}/Tools/docl/docl --lift ${CMAKE_CURRENT_SOURCE_DIR}/Neo3D.lua -o ${DOCUMENTATION_BINARY_DIR}/LuaDoc ${CMAKE_CURRENT_SOURCE_DIR}/*.lua
)
ELSE()
    message("-- Will NOT generate Lua documentation.\n--")
ENDIF()
