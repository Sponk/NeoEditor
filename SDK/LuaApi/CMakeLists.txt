## Find sources
FILE(GLOB_RECURSE SOURCES *.lua)
INSTALL(FILES ${SOURCES} DESTINATION ${NEO_INSTALL_DIR}/LuaApi)

INCLUDE(CopyFiles)
copy_files("*.lua" "${NEO_BINARY_OUTPUT}/scripts/SDK" "copy-lua-api")

IF(LUA52_FOUND)
## TODO: Document behavior!
ADD_CUSTOM_TARGET(docl ALL
  COMMAND lua ${NEO_SOURCE_DIR}/Tools/docl/docl --lift ${CMAKE_CURRENT_SOURCE_DIR}/Neo3D.lua -o ${DOCUMENTATION_BINARY_DIR}/LuaDoc ${CMAKE_CURRENT_SOURCE_DIR}/*.lua
)
ELSE()
    message("-- Will NOT generate Lua documentation. No Lua executable specified!\n--")
ENDIF()
