## Find sources
FILE(GLOB SOURCES *.lua)
INSTALL(FILES ${SOURCES} DESTINATION ${NEO_INSTALL_DIR}/LuaApi)

INCLUDE(CopyFiles)
copy_files("*.lua" "${NEO_BINARY_OUTPUT}/scripts/SDK" "copy-lua-api")

IF(NOT ${LUADOC_EXECUTABLE} STREQUAL "")
## TODO: Document behavior!
ADD_CUSTOM_TARGET(luadoc ALL
  COMMAND ${LUADOC_EXECUTABLE} -d ${DOCUMENTATION_BINARY_DIR}/LuaDoc ${SOURCES}}
)
ELSE()
    message("-- Will NOT generate LuaDoc documentation. No executable specified!\n--")
ENDIF()
