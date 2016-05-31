
add_library(SDLGame SHARED Plugin.cpp SDLGame.cpp SDLThread.cpp SDLInputContext.cpp SDLInputContext.h SDLSystem.cpp SDLSystem.h)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${SDL_INCLUDE_DIR}
)

target_link_libraries(SDLGame
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${SDL_LIBRARIES}
)

install(TARGETS SDLGame DESTINATION ${NEO_INSTALL_DIR})
