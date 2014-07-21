INSTALL(DIRECTORY Editor/font Editor/gui DESTINATION ${MARATIS_INSTALL_DIR})
INSTALL(FILES neo-editor.desktop maratis-editor.desktop DESTINATION ${MARATIS_INSTALL_DIR}/../share/applications/)
INSTALL(FILES maratis.svg neo-icon.png DESTINATION ${MARATIS_INSTALL_DIR}/../share/icons/)
