
include(Swig)

add_swig_module(Neo2DPlugin lua Neo2DPlugin.i "-I${CMAKE_CURRENT_SOURCE_DIR} -I${NEO2D_INCLUDE_DIR} -I${NEO_CORE_INCLUDE_DIR}")

add_library(Neo2DPlugin SHARED Plugin.cpp ${Neo2DPlugin_SOURCES})

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${NEO2D_INCLUDE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}
		${LUA_INCLUDE_DIR}
)

target_link_libraries(Neo2DPlugin
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${NEO2D_LIBRARIES}
		${LUA_LIBRARIES}
)

install(TARGETS Neo2DPlugin DESTINATION ${NEO_INSTALL_DIR})
