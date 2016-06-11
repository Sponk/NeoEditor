

#find_package(Mono)

if(${MONO_FOUND})

    add_library(NeoSharp SHARED Plugin.cpp MonoContext.cpp MonoContext.h MonoSubgame.cpp MonoSubgame.h)

    include_directories(
            ${NEO_CORE_INCLUDE_DIR}
            ${NEO_ENGINE_INCLUDE_DIR}
            ${MONO_INCLUDE_DIRS}
    )

    target_link_libraries(NeoSharp
            ${NEO_CORE_LIBRARIES}
            ${NEO_ENGINE_LIBRARIES}
            ${MONO_LIBRARIES}
            )

    install(TARGETS NeoSharp DESTINATION ${NEO_INSTALL_DIR})
endif(${MONO_FOUND})
