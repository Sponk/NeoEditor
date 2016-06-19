
IF(NOT NO_TOOLS)
	ADD_SUBDIRECTORY(Publisher)
	
	IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/External)
		MESSAGE("-- Found external tools!")
		ADD_SUBDIRECTORY(External)
	ENDIF()
	
	MESSAGE("-- Building tools is enabled.")
	
ELSE()
	MESSAGE("-- Building tools was disabled.")
ENDIF()
