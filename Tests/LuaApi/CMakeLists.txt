INCLUDE(CopyFiles)
copy_files("*" "${NEO_BINARY_OUTPUT}" "copy-lua-api-test")

add_custom_target(lua-api-test "${NEO_BINARY_OUTPUT}/NeoPlayer")
add_dependencies(lua-api-test all copy-lua-api-test)