INCLUDE(CopyFiles)
copy_files("*" "${NEO_BINARY_OUTPUT}" "copy-editor")

add_custom_target(editor-test "${NEO_BINARY_OUTPUT}/NeoPlayer" WORKING_DIRECTORY ${NEO_BINARY_OUTPUT})
add_dependencies(editor-test NeoPlayer Neo2D NeoCore NeoEngine copy-editor copy-editor-icon)

INSTALL(DIRECTORY assets DESTINATION ${NEO_INSTALL_DIR})

FIND_PROGRAM(MONDOC_EXECUTABLE mondoc)

IF(NOT MONDOC_EXECUTABLE)
    MESSAGE("-- Could not find Mondoc, no Lua documentation will be generated!")
ELSE()
    MESSAGE("-- Found Mondoc at " ${MONDOC_EXECUTABLE})

    FILE(GLOB_RECURSE SOURCES *.lua)
    FILE(MAKE_DIRECTORY ${DOCUMENTATION_BINARY_DIR}/Editor)

    ADD_CUSTOM_TARGET(editor-doc ALL COMMAND ${MONDOC_EXECUTABLE} -o ${DOCUMENTATION_BINARY_DIR}/Editor -i assets/editor/main.lua ${SOURCES})
    INSTALL(DIRECTORY ${DOCUMENTATION_BINARY_DIR}/Editor DESTINATION ${NEO_INSTALL_DIR}/Documentation/Editor)
ENDIF()

## Update needed translations
if(UNIX)
  add_custom_target(grep-translations-lua-editor grep -o -r "\"tr(\\\".*\\\")\"" assets > translations.txt
  WORKING_DIRECTORY ..)
ENDIF()
