
add_library(NeoEditor SHARED src/Plugin.cpp src/EditorGame.cpp src/EditorGame.h src/Tool.cpp src/Tool.h)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${NEO2D_INCLUDE_DIR}
)

target_link_libraries(NeoEditor
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${NEO2D_LIBRARIES}
)