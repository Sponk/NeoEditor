add_library(GLESRenderer SHARED Plugin.cpp GLESRenderer.cpp GLESRenderer.h)

find_package(OpenGLES2 REQUIRED)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${TINYUTF8_INCLUDE_DIR}
		${OPENGLES2_INCLUDE_DIR}
)

target_link_libraries(GLESRenderer
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${OPENGLES2_LIBRARIES}
)

include(CopyFiles)
add_definitions(-DSHADER_PATH="assets/shaders")

# Copy shaders
copy_files("*.vert" "${NEO_BINARY_OUTPUT}/assets/shaders" "copy-gles-vert-shaders")
copy_files("*.frag" "${NEO_BINARY_OUTPUT}/assets/shaders" "copy-gles-frag-shaders")

add_dependencies(GLESRenderer copy-gles-vert-shaders)
add_dependencies(GLESRenderer copy-gles-frag-shaders)

file(GLOB VERT_SHADERS *.vert)
file(GLOB FRAG_SHADERS *.frag)

install(TARGETS GLESRenderer DESTINATION ${NEO_INSTALL_DIR})
install(FILES ${VERT_SHADERS} ${FRAG_SHADERS} DESTINATION ${NEO_INSTALL_DIR}/assets/shaders)