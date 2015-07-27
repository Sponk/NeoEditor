INCLUDE(CopyFiles)
copy_files("*" "${NEO_BINARY_OUTPUT}" "copy-editor")

add_custom_target(editor-test "${NEO_BINARY_OUTPUT}/NeoPlayer" WORKING_DIRECTORY ${NEO_BINARY_OUTPUT})
add_dependencies(editor-test all NeoPlayer Neo2D NeoCore NeoEngine copy-editor)
