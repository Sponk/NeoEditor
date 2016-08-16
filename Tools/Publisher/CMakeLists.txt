
set(SOURCES src/Publisher.cpp src/Publisher.h src/NPKPublisher.cpp src/NPKPublisher.h)

add_executable(neo-publisher src/main.cpp ${SOURCES})

include_directories(
        ${NEO_CORE_INCLUDE_DIR}
        ${NEO_ENGINE_INCLUDE_DIR}

        ${NPK_INCLUDE_DIR}
        ${SEXPRESSO_INCLUDE_DIR}
        ${LIBPROJECT_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIR}
)

target_link_libraries(neo-publisher
        ${NPK_LIBRARIES}
        ${SEXPRESSO_LIBRARIES}
        ${NEO_CORE_LIBRARIES}
        ${NEO_ENGINE_LIBRARIES}
        ${LIBPROJECT_LIBRARIES}
)

if(NOT NO_TESTS)
    add_executable(PublisherTest ${SOURCES} tests/main.cpp tests/NPKTest.cpp)
    target_link_libraries(PublisherTest ${LIBPROJECT_LIBRARIES} ${SEXPRESSO_LIBRARIES}
            ${NEO_CORE_LIBRARIES} ${GTEST_LIBRARIES} ${NEO_ENGINE_LIBRARIES})

    add_dependencies(PublisherTest NPKPlugin)
endif()

install(TARGETS neo-publisher DESTINATION ${NEO_INSTALL_DIR})
