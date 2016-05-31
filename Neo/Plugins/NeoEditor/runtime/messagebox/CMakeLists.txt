
if(LINUX OR USE_QT_RUNTIME AND NOT USE_GNUSTEP_RUNTIME)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	find_package(Qt5Widgets REQUIRED)
	add_executable(messagebox main.cpp)

	include_directories(${SEXPRESSO_INCLUDE_DIR})
	target_link_libraries(messagebox Qt5::Widgets ${SEXPRESSO_LIBRARIES})
elseif(APPLE OR USE_GNUSTEP_RUNTIME)

	set(CMAKE_C_FLAGS "-x objective-c++ -std=c++11 -fconstant-string-class=NSConstantString")
	add_executable(messagebox main.m)

	include_directories(${SEXPRESSO_INCLUDE_DIR})
	target_link_libraries(messagebox ${SEXPRESSO_LIBRARIES} objc gnustep-base gnustep-gui)

elseif(WIN32)
	add_executable(messagebox win32_main.cpp)

	include_directories(${SEXPRESSO_INCLUDE_DIR})
	target_link_libraries(messagebox ${SEXPRESSO_LIBRARIES})
endif()

install(TARGETS messagebox DESTINATION ${NEO_INSTALL_DIR})
