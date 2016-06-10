
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
add_executable(ProjectManager
        src/qt/main.cpp
        src/qt/mainwindow.cpp
        src/qt/mainwindow.h
        src/qt/mainwindow.ui
        src/qt/NewProjectWizard.ui
        src/qt/NewProjectWizard.cpp
        src/qt/NewProjectWizard.h src/ProjectBackend.cpp src/ProjectBackend.h)

include_directories(${SEXPRESSO_INCLUDE_DIR} ${GTEST_INCLUDE_DIR} ${NEO_CORE_INCLUDE_DIR})
target_link_libraries(ProjectManager Qt5::Widgets ${SEXPRESSO_LIBRARIES} libproject ${NEO_CORE_LIBRARIES})

if(NOT NO_TESTS)
        add_executable(ProjectManagerTest tests/main.cpp src/ProjectBackend.cpp src/ProjectBackend.h tests/BackendTest.cpp)
        target_link_libraries(ProjectManagerTest libproject ${SEXPRESSO_LIBRARIES} ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES} ${GTEST_LIBRARIES})
endif()

install(TARGETS ProjectManager DESTINATION ${NEO_INSTALL_DIR} COMPONENT projectmanager)

if(WIN32)
    get_filename_component(QT_BINARY_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
    install(FILES
        ${QT_BINARY_DIR}/Qt5Core.dll
        ${QT_BINARY_DIR}/Qt5Gui.dll
        ${QT_BINARY_DIR}/Qt5Widgets.dll
        DESTINATION ${NEO_INSTALL_DIR} COMPONENT projectmanager)

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        install(FILES
            ${QT_BINARY_DIR}/libgcc_s_dw2-1.dll
            ${QT_BINARY_DIR}/libstdc++-6.dll
            ${QT_BINARY_DIR}/libwinpthread-1.dll
            DESTINATION ${NEO_INSTALL_DIR} COMPONENT projectmanager)
    endif()
endif()
