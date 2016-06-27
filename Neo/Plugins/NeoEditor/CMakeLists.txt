add_library(NeoEditor SHARED src/Plugin.cpp src/EditorGame.cpp src/EditorGame.h src/Tool.cpp src/Tool.h src/Sidepanel.cpp src/Sidepanel.h src/NativeToolset.cpp src/NativeToolset.h src/ExternalToolset.cpp src/ExternalToolset.h src/SceneView.cpp src/SceneView.h src/VectorEdit.h src/VectorEdit.cpp src/UndoQueue.cpp src/UndoQueue.h src/ColorEdit.h src/ColorEdit.cpp src/Console.h src/Console.cpp)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${NEO2D_INCLUDE_DIR}
		${SEXPRESSO_INCLUDE_DIR}
		../../libproject
		${GTEST_INCLUDE_DIR}
)

target_link_libraries(NeoEditor
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${NEO2D_LIBRARIES}
		${SEXPRESSO_LIBRARIES}
		libproject
)


if(NOT NO_TESTS)
	add_executable(NeoEditorTest tests/main.cpp tests/UndoQueueTest.cpp src/UndoQueue.cpp)
	target_link_libraries(NeoEditorTest ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES} ${GTEST_LIBRARIES})
endif()

add_subdirectory(runtime)

install(TARGETS NeoEditor DESTINATION ${NEO_INSTALL_DIR})
install(DIRECTORY data DESTINATION ${NEO_INSTALL_DIR})
install(FILES plugins.cfg default.ttf DESTINATION ${NEO_INSTALL_DIR}/assets)
copy_files("data/*" "${NEO_BINARY_OUTPUT}" "copy-editor-data")
