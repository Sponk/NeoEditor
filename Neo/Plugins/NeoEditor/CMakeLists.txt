add_library(NeoEditor SHARED src/Plugin.cpp src/EditorGame.cpp src/EditorGame.h src/Tool.cpp src/Tool.h src/Sidepanel.cpp src/Sidepanel.h src/NativeToolset.cpp src/NativeToolset.h src/ExternalQtToolset.cpp src/ExternalQtToolset.h src/SceneView.cpp src/SceneView.h)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${NEO2D_INCLUDE_DIR}
		${SEXPRESSO_INCLUDE_DIR}
)

target_link_libraries(NeoEditor
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${NEO2D_LIBRARIES}
		${SEXPRESSO_LIBRARIES}
)

## Uncomment to enable experimental Qt tools
add_subdirectory(runtime)

install(DIRECTORY data DESTINATION ${NEO_INSTALL_DIR})
copy_files("*" "${NEO_BINARY_OUTPUT}" "copy-editor-data")
