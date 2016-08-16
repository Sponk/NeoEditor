
set(SOURCES
	src/Plugin.cpp
	src/EditorGame.cpp
	src/Tool.cpp
	src/Sidepanel.cpp
	src/NativeToolset.cpp
	src/ExternalToolset.cpp
	src/SceneView.cpp
	src/BehaviorContainer.cpp
	src/VectorEdit.cpp
	src/UndoQueue.cpp
	src/ColorEdit.cpp
	src/Console.cpp
)

set(INCLUDES
	src/LuaInterface.h
	src/EditorGame.h
	src/Tool.h
	src/Sidepanel.h
	src/ExternalToolset.h
	src/NativeToolset.h
	src/SceneView.h
	src/VectorEdit.h
	src/Console.h
	src/UndoQueue.h
	src/ColorEdit.h
	src/BehaviorContainer.h
)

if(SWIG_FOUND)
	set_source_files_properties(NeoEditor.i PROPERTIES CPLUSPLUS ON)
	#SET_SOURCE_FILES_PROPERTIES(NeoEditor.i PROPERTIES SWIG_FLAGS -c++ -includeall -ignoremissing)

	swig_add_module(NeoEditor lua NeoEditor.i ${SOURCES} ${INCLUDES})
			target_link_libraries(NeoEditor
			${NEO_CORE_LIBRARIES}
			${NEO_ENGINE_LIBRARIES}
			${NEO2D_LIBRARIES}
			${SEXPRESSO_LIBRARIES}
			libproject
			${LUA_LIBRARIES})
else()
	add_library(NeoEditor SHARED ${SOURCES} ${INCLUDES})
	target_link_libraries(NeoEditor
			${NEO_CORE_LIBRARIES}
			${NEO_ENGINE_LIBRARIES}
			${NEO2D_LIBRARIES}
			${SEXPRESSO_LIBRARIES}
			libproject
	)
endif()

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${NEO2D_INCLUDE_DIR}
		${SEXPRESSO_INCLUDE_DIR}
		../../libproject
		${GTEST_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${LUA_INCLUDE_DIR}
)

if(NOT NO_TESTS)
	add_executable(NeoEditorTest tests/main.cpp tests/UndoQueueTest.cpp src/UndoQueue.cpp)
	target_link_libraries(NeoEditorTest ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES} ${GTEST_LIBRARIES})
endif()

add_subdirectory(runtime)

install(TARGETS NeoEditor DESTINATION ${NEO_INSTALL_DIR})
install(DIRECTORY data DESTINATION ${NEO_INSTALL_DIR})
install(FILES default.ttf DESTINATION ${NEO_INSTALL_DIR}/assets)
install(FILES editor.nproj DESTINATION ${NEO_INSTALL_DIR})

copy_files("data/*" "${NEO_BINARY_OUTPUT}" "copy-editor-data")
