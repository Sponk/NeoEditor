## Find source and headers
FILE(GLOB_RECURSE HEADERS *.h)
FILE(GLOB_RECURSE SOURCES *.c*)

IF(CMAKE_HAS_SORT)
  LIST(SORT HEADERS)
  LIST(SORT SOURCES)
ENDIF(CMAKE_HAS_SORT)

## Build
INCLUDE_DIRECTORIES(
    ${FLTK_INCLUDE_DIR}
    ${RAKNET_INCLUDE_DIR}
    ${NEO_COMMON_INCLUDE_DIR}
    ${NEO_ENGINE_INCLUDE_DIR}
    ${NEO_CORE_INCLUDE_DIR}
)

ADD_DEFINITIONS(-DREPORT_DOMAIN="scary-squid.de")

# SET(CMAKE_INSTALL_RPATH "./")
ADD_EXECUTABLE(CrashHandler ${SOURCES} ${HEADERS})

TARGET_LINK_LIBRARIES(CrashHandler
  ${FLTK_LIBRARIES}
  ${FLTK_DEPENDENCIES}
  ${NEO_CORE_LIBRARIES}
  ${NEO_ENGINE_LIBRARIES}
  ${RAKNET_LIBRARIES}
  ${NEO_COMMON_LIBRARIES}
  ${WINSOCK_LIBRARIES}
)

INSTALL(TARGETS CrashHandler DESTINATION ${NEO_INSTALL_DIR})

