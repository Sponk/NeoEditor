INSTALL(DIRECTORY Editor/font Editor/gui DESTINATION ${NEO_INSTALL_DIR})

IF(NOT WIN32)
INSTALL(FILES neo-editor.desktop maratis-editor.desktop DESTINATION ${NEO_INSTALL_DIR}/../share/applications/)
INSTALL(FILES maratis.svg neo-icon.png DESTINATION ${NEO_INSTALL_DIR}/../share/icons/)
ENDIF()

ADD_SUBDIRECTORY(Editor/gui)
