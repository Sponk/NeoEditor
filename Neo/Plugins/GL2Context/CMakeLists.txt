
add_library(GL2Context SHARED Plugin.cpp GLContext.cpp)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${GLEW_INCLUDE_DIR}
)

target_link_libraries(GL2Context
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${OPENGL_LIBRARIES}
		${GLEW_LIBRARIES}
)

install(TARGETS GL2Context DESTINATION ${NEO_INSTALL_DIR})
