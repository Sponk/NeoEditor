
add_library(StandardRenderer SHARED Plugin.cpp StandardRenderer.cpp)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${TINYUTF8_INCLUDE_DIR}
)

target_link_libraries(StandardRenderer
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
)

include(CopyFiles)
add_definitions(-DSHADER_PATH="assets/shaders" -DGL_DEBUG)

# Copy shaders
copy_files("*.vert" "${NEO_BINARY_OUTPUT}/assets/shaders" "copy-vert-shaders")
copy_files("*.frag" "${NEO_BINARY_OUTPUT}/assets/shaders" "copy-frag-shaders")

file(GLOB VERT_SHADERS *.vert)
file(GLOB FRAG_SHADERS *.frag)

install(TARGETS StandardRenderer DESTINATION ${NEO_INSTALL_DIR})
install(FILES ${VERT_SHADERS} ${FRAG_SHADERS} DESTINATION ${NEO_INSTALL_DIR}/assets/shaders)