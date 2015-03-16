INSTALL(DIRECTORY Editor/font Editor/gui DESTINATION ${NEO_INSTALL_DIR})

IF(NOT WIN32)
INSTALL(FILES neo-editor.desktop neo-project-manager.desktop neo-store.desktop DESTINATION share/applications) # ${NEO_INSTALL_DIR}/../share/applications/)
INSTALL(FILES maratis.svg neo-icon.png DESTINATION share/icons) # ${NEO_INSTALL_DIR}/../share/icons/)
ENDIF()

ADD_SUBDIRECTORY(Editor/gui)
