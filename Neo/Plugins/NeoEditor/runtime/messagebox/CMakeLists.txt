
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
add_executable(messagebox main.cpp)

include_directories(${SEXPRESSO_INCLUDE_DIR})
target_link_libraries(messagebox Qt5::Widgets ${SEXPRESSO_LIBRARIES})

