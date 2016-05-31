
add_executable(NeoPlayer2 src/main.cpp src/Debug.cpp)

if(WIN32)
  set(NEO_IMAGEHELP "imagehlp")
endif(WIN32)

include_directories(
	${NEO_CORE_INCLUDE_DIR}
	${NEO_ENGINE_INCLUDE_DIR}
	${SEXPRESSO_INCLUDE_DIR}
)

target_link_libraries( NeoPlayer2
	${NEO_CORE_LIBRARIES}
	${NEO_ENGINE_LIBRARIES}
	${SEXPRESSO_LIBRARIES}
	${NEO_IMAGEHELP}
)

install(TARGETS NeoPlayer2 DESTINATION ${NEO_INSTALL_DIR})
