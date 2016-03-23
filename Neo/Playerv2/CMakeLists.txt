
add_executable(NeoPlayer2 src/main.cpp)

include_directories(
	${NEO_CORE_INCLUDE_DIR}
	${NEO_ENGINE_INCLUDE_DIR}
)

target_link_libraries( NeoPlayer2
	${NEO_CORE_LIBRARIES}
	${NEO_ENGINE_LIBRARIES}
)