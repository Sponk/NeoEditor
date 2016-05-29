
if(LINUX OR USE_QT_RUNTIME AND NOT USE_GNUSTEP_RUNTIME)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	find_package(Qt5Widgets REQUIRED)
	add_executable(filedialog main.cpp)

	include_directories(${SEXPRESSO_INCLUDE_DIR})
	target_link_libraries(filedialog Qt5::Widgets ${SEXPRESSO_LIBRARIES})
elseif(APPLE OR USE_GNUSTEP_RUNTIME)
	set(CMAKE_C_FLAGS "-x objective-c++ -std=c++11 -fconstant-string-class=NSConstantString")
	add_executable(filedialog main.m)

	include_directories(${SEXPRESSO_INCLUDE_DIR})
	target_link_libraries(filedialog ${SEXPRESSO_LIBRARIES} objc gnustep-base gnustep-gui)
elseif(WIN32)
	add_executable(filedialog win32_main.cpp)

	include_directories(${SEXPRESSO_INCLUDE_DIR})
	target_link_libraries(filedialog ${SEXPRESSO_LIBRARIES})
endif()


