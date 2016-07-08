
if(SWIG_FOUND)
  add_swig_module(NeoEditor lua NeoEditor.i "")
endif()

add_library(NeoEditor SHARED
  ${NeoEditor_SOURCES}
  src/LuaInterface.h
  src/Plugin.cpp
  src/EditorGame.cpp
  src/EditorGame.h
  src/Tool.cpp
  src/Tool.h
  src/Sidepanel.cpp
  src/Sidepanel.h
  src/NativeToolset.cpp
  src/NativeToolset.h
  src/ExternalToolset.cpp
  src/ExternalToolset.h
  src/SceneView.cpp
  src/SceneView.h
  src/VectorEdit.h
  src/VectorEdit.cpp
  src/UndoQueue.cpp
  src/UndoQueue.h
  src/ColorEdit.h
  src/ColorEdit.cpp
  src/Console.h
  src/Console.cpp
  src/BehaviorContainer.h
  src/BehaviorContainer.cpp)

if(SWIG_FOUND)
  add_dependencies(NeoEditor NeoEditor-lua-swig)
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

target_link_libraries(NeoEditor
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${NEO2D_LIBRARIES}
		${SEXPRESSO_LIBRARIES}
		libproject
		${LUA_LIBRARIES}
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
