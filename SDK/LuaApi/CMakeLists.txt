## Find sources
FILE(GLOB SOURCES *.lua)
INSTALL(FILES ${SOURCES} DESTINATION ${NEO_INSTALL_DIR}/LuaApi)

INCLUDE(CopyFiles)
copy_files("*.lua" "${NEO_BINARY_OUTPUT}/scripts/SDK" "copy-lua-api")

IF(LUA51_FOUND)
## TODO: Document behavior!
ADD_CUSTOM_TARGET(docl ALL
  COMMAND lua ${NEO_SOURCE_DIR}/Tools/docl/docl -o ${DOCUMENTATION_BINARY_DIR}/LuaDoc ${SOURCES}
)
ELSE()
    message("-- Will NOT generate Lua documentation. No Lua executable specified!\n--")
ENDIF()
