## Build TODO: Only when editor is built!
ADD_DEFINITIONS(-DM_PACKAGE_WRITABLE -DNEO2D_DLL)
SET(CMAKE_SHARED_LIBRARY_PREFIX "")

## Build
INCLUDE_DIRECTORIES(
		${NEO_COMMON_INCLUDE_DIR}
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${GTEST_INCLUDE_DIR}
		${TINYUTF8_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/utfcpp/source
)

SET(HEADERS
	include/Neo2D.h
	include/Object2D.h
	include/Container.h
	include/Widget.h
	include/LayoutStrategy.h
	include/FilterStrategy.h
	include/Event.h
	include/CommonEvents.h
	include/Canvas.h
	include/Button.h
	include/Neo2DLevel.h
	include/Label.h
	include/CheckButton.h
	include/RadioButton.h
	include/EditField.h
	include/Menu.h
	include/ImageButton.h
	include/Sprite.h
	include/Toolbar.h
	include/Scrollbar.h
	include/ScrollPanel.h
	include/TreeView.h
	include/Translator.h
	include/csv.h
	include/HorizontalLayout.h
	include/VerticalLayout.h
	include/KeyboardShortcuts.h)

SET(SOURCES
	src/Object2D.cpp
	src/Widget.cpp
	src/Event.cpp
	src/Neo2D.cpp
	src/Canvas.cpp
	src/Button.cpp
	src/Neo2DLevel.cpp
	src/Label.cpp
	src/CheckButton.cpp
	src/RadioButton.cpp
	src/EditField.cpp
	src/Menu.cpp
	src/ImageButton.cpp
	src/Sprite.cpp
	src/Toolbar.cpp
	src/Scrollbar.cpp
	src/ScrollPanel.cpp
	src/Container.cpp
	src/TreeView.cpp
	src/Translator.cpp
	src/HorizontalLayout.cpp
	src/VerticalLayout.cpp
	src/KeyboardShortcuts.cpp)

IF(NOT ANDROID AND NOT EMSCRIPTEN)
	ADD_LIBRARY(Neo2D SHARED ${SOURCES} ${HEADERS})
ELSEIF(ANDROID)
	ADD_LIBRARY(Neo2D STATIC ${SOURCES} ${HEADERS})
ELSEIF(EMSCRIPTEN)

ENDIF()

IF(NOT NO_TESTS)
	ADD_EXECUTABLE(Neo2DTest tests/main.cpp tests/Object2DTest.cpp tests/ContainerTest.cpp tests/EventTest.cpp tests/ButtonTest.cpp tests/CheckButtonTest.cpp tests/RadioButtonTest.cpp tests/EditFieldTest.cpp tests/MenuTest.cpp tests/ScrollbarTest.cpp tests/TreeViewTest.cpp tests/VerticalLayoutTest.cpp tests/KeyboardShortcutsTest.cpp)
	TARGET_LINK_LIBRARIES(Neo2DTest Neo2D ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES} ${GTEST_LIBRARIES})
ENDIF()

# Needed for windows
TARGET_LINK_LIBRARIES(Neo2D ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES})

INSTALL(TARGETS Neo2D DESTINATION ${NEO_INSTALL_DIR})
INSTALL(TARGETS Neo2D DESTINATION ${NEO_INSTALL_DIR}/SDK/lib COMPONENT sdk)
INSTALL(FILES ${HEADERS} DESTINATION ${NEO_INSTALL_DIR}/SDK/include/Neo2D COMPONENT sdk)
