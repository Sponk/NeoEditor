
add_library(MobileRenderer SHARED Plugin.cpp MobileRenderer.cpp)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${TINYUTF8_INCLUDE_DIR}
)

target_link_libraries(MobileRenderer
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
)

install(TARGETS MobileRenderer DESTINATION ${NEO_INSTALL_DIR})
