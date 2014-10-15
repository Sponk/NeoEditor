INCLUDE(CopyFiles)
copy_files("*" "${MARATIS_BINARY_OUTPUT}" "copy-lua-api-test")

add_custom_target(lua-api-test "${MARATIS_BINARY_OUTPUT}/MaratisPlayer")
add_dependencies(lua-api-test copy-lua-api-test)
