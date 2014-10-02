FILE(GLOB SCRIPTS *.ini)

INSTALL(FILES ${SCRIPTS} DESTINATION ${MARATIS_INSTALL_DIR}/translations)

INCLUDE(CopyFiles)
copy_files("*.ini" "${MARATIS_BINARY_OUTPUT}/translations" "NeoTranslations")
