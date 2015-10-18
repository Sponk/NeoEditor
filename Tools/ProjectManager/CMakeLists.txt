INCLUDE(CopyFiles)
copy_files("*" "${NEO_BINARY_OUTPUT}" "copy-project-manager")

add_custom_target(project-manager-test "${NEO_BINARY_OUTPUT}/NeoPlayer" -e ProjectManager/main.lua WORKING_DIRECTORY ${NEO_BINARY_OUTPUT})
add_dependencies(project-manager-test all NeoPlayer Neo2D NeoCore NeoEngine copy-project-manager copy-editor)

INSTALL(DIRECTORY ProjectManager DESTINATION ${NEO_INSTALL_DIR})
FIND_PROGRAM(MONDOC_EXECUTABLE mondoc)

IF(NOT MONDOC_EXECUTABLE)
    MESSAGE("-- Could not find Mondoc, no Lua documentation for the ProjectManager will be generated!")
ELSE()
    FILE(GLOB_RECURSE SOURCES *.lua)
    FILE(MAKE_DIRECTORY ${DOCUMENTATION_BINARY_DIR}/ProjectManager)

    ADD_CUSTOM_TARGET(project-manager-doc ALL COMMAND ${MONDOC_EXECUTABLE} -o ${DOCUMENTATION_BINARY_DIR}/ProjectManager -i ${CMAKE_CURRENT_SOURCE_DIR}/ProjectManager/main.lua ${SOURCES})
    INSTALL(DIRECTORY ${DOCUMENTATION_BINARY_DIR}/ProjectManager DESTINATION ${NEO_INSTALL_DIR}/Documentation/ProjectManager)
ENDIF()

## Update needed translations
if(UNIX)
  add_custom_target(grep-translations-lua-project-manager grep -o -r "\"tr(\\\".*\\\")\"" ./ProjectManager > translations.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
ENDIF()
