FILE(GLOB SCRIPTS *.lua)

INSTALL(FILES ${SCRIPTS} DESTINATION ${MARATIS_INSTALL_DIR}/plugins)

INCLUDE(CopyFiles)
copy_files("*.*" "${MARATIS_BINARY_OUTPUT}/plugins" "NeoPlugins")
copy_files("libs/*" "${MARATIS_BINARY_OUTPUT}/plugins" "NeoPluginsLibs")

ADD_SUBDIRECTORY(libs)
