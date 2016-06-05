set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(libproject STATIC Project.cpp Project.h)

include_directories(
		${SEXPRESSO_INCLUDE_DIR}
		${NEO_CORE_INCLUDE_DIR}
)

target_link_libraries(libproject
		${SEXPRESSO_LIBRARIES}
)
