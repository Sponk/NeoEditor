add_library(GL3Renderer SHARED Plugin.cpp GL3Renderer.cpp GL3Renderer.h)

find_package(OpenGL REQUIRED)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${TINYUTF8_INCLUDE_DIR}
		${OPENGL_INCLUDE_DIR}
		${GLEW_INCLUDE_DIR}
)

target_link_libraries(GL3Renderer
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${OPENGL_LIBRARIES}
		${GLEW_LIBRARIES}
)

include(CopyFiles)
add_definitions(-DSHADER_PATH="assets/shaders" -DGL_DEBUG)

# Copy shaders
copy_files("*.vert" "${NEO_BINARY_OUTPUT}/assets/shaders" "copy-gl3-vert-shaders")
copy_files("*.frag" "${NEO_BINARY_OUTPUT}/assets/shaders" "copy-gl3-frag-shaders")

add_dependencies(GL3Renderer copy-gl3-vert-shaders)
add_dependencies(GL3Renderer copy-gl3-frag-shaders)

file(GLOB VERT_SHADERS *.vert)
file(GLOB FRAG_SHADERS *.frag)

install(TARGETS GL3Renderer DESTINATION ${NEO_INSTALL_DIR})
install(FILES ${VERT_SHADERS} ${FRAG_SHADERS} DESTINATION ${NEO_INSTALL_DIR}/assets/shaders)
