FILE(GLOB_RECURSE HEADERS *.h)
FILE(GLOB_RECURSE SOURCES sexpresso/*.c*)

if(EMSCRIPTEN)
add_definitions(-std=c++11)
endif()

add_library(sexpresso STATIC ${SOURCES} ${HEADERS}) 
