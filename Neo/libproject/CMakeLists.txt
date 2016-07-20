set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(libproject STATIC Project.cpp Project.h)

include_directories(
		${GTEST_INCLUDE_DIR}
		${SEXPRESSO_INCLUDE_DIR}
		${NEO_CORE_INCLUDE_DIR}
)

target_link_libraries(libproject
		${SEXPRESSO_LIBRARIES}
)

if(NOT NO_TESTS)
	add_executable(ProjectTest tests/main.cpp tests/ProjectTest.cpp)
	target_link_libraries(ProjectTest libproject ${SEXPRESSO_LIBRARIES} ${NEO_CORE_LIBRARIES} ${GTEST_LIBRARIES})
endif()
