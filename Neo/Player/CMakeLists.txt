## Find source and headers
FILE(GLOB_RECURSE HEADERS *.h)
FILE(GLOB_RECURSE SOURCES *.c*)

IF(CMAKE_HAS_SORT)
  LIST(SORT HEADERS)
  LIST(SORT SOURCES)
ENDIF(CMAKE_HAS_SORT)

## Build
INCLUDE_DIRECTORIES(
  ${NEO_COMMON_INCLUDE_DIR}
  ${NEO_CORE_INCLUDE_DIR}
  ${NEO_ENGINE_INCLUDE_DIR}
  ${NEO2D_INCLUDE_DIR}

  ## 3rdparty include directories
  ${BULLET_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${JPEG_INCLUDE_DIR}
  ${PNG_INCLUDE_DIR}
  ${LUA_INCLUDE_DIR}
  ${NPK_INCLUDE_DIR}
  ${SDL_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${TINYXML_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
)

# SET(CMAKE_INSTALL_RPATH "./")

ADD_DEFINITIONS(-DPLAYER_VERSION_STRING=${NEO_VERSION_STRING})

IF(NOT EMSCRIPTEN)
ADD_EXECUTABLE(NeoPlayer ${SOURCES} ${HEADERS})
ELSE()
ADD_EXECUTABLE(NeoPlayer.html ${SOURCES} ${HEADERS})
ENDIF()

IF(ANDROID)

list(APPEND PLATFORM_LIBRARIES ${ANDROID_LIBDIR}/)

ENDIF()

IF(NOT ANDROID AND NOT EMSCRIPTEN)
TARGET_LINK_LIBRARIES(NeoPlayer
  ${NEO_COMMON_LIBRARIES}
  ${NEO_CORE_LIBRARIES}
  ${NEO_ENGINE_LIBRARIES}
  ${NEO2D_LIBRARIES}

  ## 3rdparty libraries
  ${BULLET_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${PNG_LIBRARIES}
  ${VORBIS_LIBRARIES}
  ${OGG_LIBRARIES}
  ${LUA_LIBRARIES}
  ${NPK_LIBRARIES}
  ${SDL_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${OPENAL_LIBRARIES}
  ${ZLIB_LIBRARIES}

  ## System libraries
  ${OPENGL_LIBRARIES}
  ${PLATFORM_LIBRARIES}
  ${ASSIMP_LIBRARIES}
)
ELSEIF(NOT EMSCRIPTEN AND ANDROID)
TARGET_LINK_LIBRARIES(NeoPlayer
  ## Maratis libraries
  ${NEO_COMMON_LIBRARIES}
  ${NEO_CORE_LIBRARIES}
  ${NEO_ENGINE_LIBRARIES}
  ${NEO2D_LIBRARIES}

  ## 3rdparty libraries
  ${BULLET_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${PNG_LIBRARIES}
  ${VORBIS_LIBRARIES}
  ${OGG_LIBRARIES}
  ${LUA_LIBRARIES}
  ${NPK_LIBRARIES}
  ${SDL_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${OPENAL_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${ASSIMP_LIBRARIES}

  ## System libraries
  ${OPENGLES2_LIBRARIES}
  ${PLATFORM_LIBRARIES}
  "${ANDROID_LIBDIR}/crtend_so.o"  
)
ELSE()
TARGET_LINK_LIBRARIES(NeoPlayer.html
  ${NEO_COMMON_LIBRARIES}
  ${NEO_CORE_LIBRARIES}
  ${NEO_ENGINE_LIBRARIES}
  ${NEO2D_LIBRARIES}

  ## 3rdparty libraries
  ${BULLET_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${PNG_LIBRARIES}
  ${VORBIS_LIBRARIES}
  ${OGG_LIBRARIES}
  ${LUA_LIBRARIES}
  ${NPK_LIBRARIES}
  ${SDL_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${OPENAL_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${ASSIMP_LIBRARIES}
  
  ## System libraries
  ${OPENGLES2_LIBRARIES}
  ${PLATFORM_LIBRARIES}
)
ENDIF()

IF(NOT EMSCRIPTEN)
INSTALL(TARGETS NeoPlayer DESTINATION ${NEO_INSTALL_DIR})
ENDIF()
