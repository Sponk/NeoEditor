FILE(GLOB SCRIPTS *.csv)

INSTALL(FILES ${SCRIPTS} DESTINATION ${NEO_INSTALL_DIR}/translations)

INCLUDE(CopyFiles)
copy_files("*.csv" "${NEO_BINARY_OUTPUT}/translations" "NeoTranslations")
