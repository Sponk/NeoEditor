
add_library(OpenALContext SHARED Plugin.cpp ALContext.cpp)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${OPENAL_INCLUDE_DIR}
)

target_link_libraries(OpenALContext
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${OPENAL_LIBRARIES}
)

install(TARGETS OpenALContext DESTINATION ${NEO_INSTALL_DIR})
