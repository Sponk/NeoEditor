INCLUDE(CopyFiles)
copy_files("*" "${NEO_BINARY_OUTPUT}" "copy-editor")

add_custom_target(editor-test "${NEO_BINARY_OUTPUT}/NeoPlayer")
add_dependencies(editor-test NeoPlayer copy-editor)
