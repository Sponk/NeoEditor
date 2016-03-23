
add_library(LuaScripting SHARED Plugin.cpp LuaScript.cpp LuaBehavior.cpp)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${LUA_INCLUDE_DIR}
)

target_link_libraries( LuaScripting
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${LUA_LIBRARIES}
)
