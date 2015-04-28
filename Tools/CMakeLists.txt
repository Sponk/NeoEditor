
IF(NOT NO_TOOLS)
	ADD_SUBDIRECTORY(CrashHandler)
	ADD_SUBDIRECTORY(ProfileViewer)
	ADD_SUBDIRECTORY(NeoStore)
	ADD_SUBDIRECTORY(ProjectManager)
	ADD_SUBDIRECTORY(ProjectWizard)
	ADD_SUBDIRECTORY(Publisher)
	ADD_SUBDIRECTORY(MonDoc)
	
	MESSAGE("-- Building tools is enabled.")
	
ELSE()
	MESSAGE("-- Building tools was disabled.")
ENDIF()