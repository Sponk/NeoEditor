
set(CMAKE_EXECUTABLE_SUFFIX ".html")

file(GLOB NEO_ENGINE_SRC ${NEO_ENGINE_INCLUDE_DIR}/../src/*.cpp)
file(GLOB NEO_CORE_SRC ${NEO_CORE_INCLUDE_DIR}/../src/*.cpp)
file(GLOB NEO2D_SRC ${NEO2D_INCLUDE_DIR}/../src/*.cpp)

file(GLOB SDLGAME_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/SDLGame/*.cpp)
list(REMOVE_ITEM SDLGAME_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/SDLGame/Plugin.cpp)
list(REMOVE_ITEM SDLGAME_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/SDLGame/SDLThread.cpp)

set(GLES_RENDERER_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/GLESRenderer/GLESRenderer.cpp)

file(GLOB ASSET_LOADING_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/AssetLoading/*.cpp)
list(REMOVE_ITEM ASSET_LOADING_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/AssetLoading/Plugin.cpp)

file(GLOB LUA_SCRIPTING_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/LuaScripting/*.cpp)
list(REMOVE_ITEM LUA_SCRIPTING_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/LuaScripting/Plugin.cpp)

file(GLOB NPK_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/NPK/*.cpp)
list(REMOVE_ITEM NPK_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/NPK/Plugin.cpp)

set(LIBPROJECT_SRC ${CMAKE_SOURCE_DIR}/Neo/libproject/Project.cpp)

file(GLOB BULLET_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/BulletPhysics/*.cpp)
list(REMOVE_ITEM BULLET_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/BulletPhysics/Plugin.cpp)

file(GLOB OPENAL_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/OpenALContext/*.cpp)
list(REMOVE_ITEM OPENAL_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/OpenALContext/Plugin.cpp)

## TODO: Make option!
file(GLOB EDITOR_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/NeoEditor/src/*.cpp)
list(REMOVE_ITEM EDITOR_SRC ${CMAKE_SOURCE_DIR}/Neo/Plugins/NeoEditor/src/Plugin.cpp)

add_executable(NeoWeb
  LoadPlugins.cpp
  
  ${NEO_ENGINE_SRC}
  ${NEO_CORE_SRC}
  ${NEO2D_SRC}
  ${SDLGAME_SRC}
  ${GLES_RENDERER_SRC}
  ${ASSET_LOADING_SRC}
  ${LUA_SCRIPTING_SRC}
  ${NPK_SRC}
  ${LIBPROJECT_SRC}
  ${BULLET_SRC}
  ${OPENAL_SRC}
  ${EDITOR_SRC}
)

include_directories(
	${NEO_CORE_INCLUDE_DIR}
	${NEO_ENGINE_INCLUDE_DIR}
	${NEO2D_INCLUDE_DIR}
	${NEO2D_INCLUDE_DIR}/../utfcpp/source
	${TINYUTF8_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIR}
	${TINYXML_INCLUDE_DIR}
	${SEXPRESSO_INCLUDE_DIR}
	${JPEG_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
	${NPK_INCLUDE_DIR}
	../Neo/Plugins/NPK
	../Neo/Plugins/AssetLoading/include
	${LIBPROJECT_INCLUDE_DIR}
)

target_link_libraries(NeoWeb Neo2DLua NeoEngineLua ${ASSIMP_LIBRARIES} ${SEXPRESSO_LIBRARIES} ${TINYXML_LIBRARIES} ${JPEG_LIBRARIES} ${LUA_LIBRARIES} ${NPK_LIBRARIES})

include(CopyFiles)
file(COPY "../Neo/Plugins/GLESRenderer/gles.vert" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders")
file(COPY "../Neo/Plugins/GLESRenderer/gles.frag" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders")

add_definitions(-s ASSERTIONS=1 -std=c++11 -s USE_BULLET=1 -s USE_SDL=2 -s FULL_ES2=1 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_VORBIS=1 -s USE_FREETYPE=1 -DSHADER_PATH="assets/shaders")
set_target_properties(NeoWeb PROPERTIES LINK_FLAGS
			"-s ASSERTIONS=1 -s USE_SDL=2 -s FULL_ES2=1 \
             -s USE_BULLET=1 -s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_VORBIS=1 -s USE_FREETYPE=1 \
			--preload-file ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/gles.vert --preload-file ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/gles.frag")

install(TARGETS NeoWeb DESTINATION ${CMAKE_INSTALL_PREFIX})

set_source_files_properties(${CMAKE_SOURCE_DIR}/SDK/NeoEngine/swig/NeoEngineLua.i PROPERTIES CPLUSPLUS ON)
set(CMAKE_SWIG_FLAGS -Doverride)
swig_add_module(NeoEngineLua lua ${CMAKE_SOURCE_DIR}/SDK/NeoEngine/swig/NeoEngineLua.i)
target_include_directories(NeoEngineLua PUBLIC ${LUA_INCLUDE_DIR} ${NEO_CORE_INCLUDE_DIR} ${NEO_ENGINE_INCLUDE_DIR})

set_source_files_properties(${CMAKE_SOURCE_DIR}/SDK/Neo2D/Neo2D.i PROPERTIES CPLUSPLUS ON)
swig_add_module(Neo2DLua lua ${CMAKE_SOURCE_DIR}/SDK/Neo2D/Neo2D.i)
target_include_directories(Neo2DLua PUBLIC ${LUA_INCLUDE_DIR} ${NEO2D_INCLUDE_DIR} ${NEO_ENGINE_INCLUDE_DIR} ${NEO_CORE_INCLUDE_DIR})
