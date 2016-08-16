
IF(NOT NO_TOOLS)
	ADD_SUBDIRECTORY(Publisher)	
	MESSAGE("-- Building tools is enabled.")
	
ELSE()
	MESSAGE("-- Building tools was disabled.")
ENDIF()
