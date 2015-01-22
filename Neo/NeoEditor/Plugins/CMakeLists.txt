FILE(GLOB SCRIPTS *.lua)

INSTALL(FILES ${SCRIPTS} DESTINATION ${NEO_INSTALL_DIR}/plugins)

INCLUDE(CopyFiles)
copy_files("*.*" "${NEO_BINARY_OUTPUT}/plugins" "NeoPlugins")
copy_files("libs/*" "${NEO_BINARY_OUTPUT}/plugins" "NeoPluginsLibs")

ADD_SUBDIRECTORY(libs)
