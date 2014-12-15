FILE(GLOB SCRIPTS *.ini)

INSTALL(FILES ${SCRIPTS} DESTINATION ${NEO_INSTALL_DIR}/translations)

INCLUDE(CopyFiles)
copy_files("*.ini" "${NEO_BINARY_OUTPUT}/translations" "NeoTranslations")
