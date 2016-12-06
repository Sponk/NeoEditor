
SET(CMAKE_SHARED_LIBRARY_PREFIX "")
ADD_DEFINITIONS(-UNEO_ENGINE_DLL)

INCLUDE_DIRECTORIES(
        ${NEO_CORE_INCLUDE_DIR}
        ${NEO_ENGINE_INCLUDE_DIR}
        ${SDL_INCLUDE_DIR}
        ${LUA_INCLUDE_DIR}
        ${MONO_INCLUDE_DIR}
)

set(JAVA_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/java)
set(MONO_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mono)

file(REMOVE ${JAVA_OUTPUT})
file(MAKE_DIRECTORY ${JAVA_OUTPUT})

file(REMOVE ${MONO_OUTPUT})
file(MAKE_DIRECTORY ${MONO_OUTPUT})

IF (${SWIG_FOUND})
    MESSAGE("-- Generating script bindings for NeoEngine")

    SET(CMAKE_SHARED_LIBRARY_PREFIX "")

    if (${CSHARP_FOUND})
        if (WIN32)
            set(DLLIMPORT ./NeoSharpNativeEngine.dll)
        else ()
            set(DLLIMPORT ./NeoSharpNativeEngine.so)
        endif ()

        set_source_files_properties(NeoSharpEngine.i PROPERTIES CPLUSPLUS ON)
        set(CMAKE_SWIG_OUTDIR ${MONO_OUTPUT})
        set(SWIG_ARGS -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR} -dllimport ${DLLIMPORT} -Doverride)
        set_property(SOURCE NeoSharpEngine.i PROPERTY SWIG_MODULE_NAME Neo)

        swig_add_module(NeoSharpEngine csharp NeoSharpEngine.i)

        swig_link_libraries(NeoSharpEngine ${NEO_ENGINE_LIBRARIES} ${NEO_CORE_LIBRARIES} ${MONO_LIBRARIES})
        target_include_directories(NeoSharpEngine PUBLIC ${MONO_INCLUDE_DIR} ${NEO_CORE_INCLUDE_DIR} ${NEO_ENGINE_INCLUDE_DIR})

        ##set(SWIG_ARGS -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR} -dllimport ${DLLIMPORT} -Doverride)
        ##add_swig_module(NeoSharpEngine csharp NeoSharpEngine.i "${SWIG_ARGS}")
        add_custom_target(NeoSharpEngineCLI ALL ${CSHARP_COMPILER} -t:library -out:${NEO_BINARY_OUTPUT}/NeoSharpEngine.dll ${MONO_OUTPUT}/*.cs)

        ##add_library(NeoSharpNativeEngine SHARED ${NeoSharpEngine_SOURCES})
        ##include_directories(${NEO_CORE_INCLUDE_DIR} ${NEO_ENGINE_INCLUDE_DIR} ${MONO_INCLUDE_DIRS})

        ##target_link_libraries(NeoSharpNativeEngine
         ##       ${NEO_CORE_LIBRARIES}
         ##       ${NEO_ENGINE_LIBRARIES}
         ##       ${MONO_LIBRARIES}
         ##       )

        ##add_dependencies(build-neo-sharp-engine NeoSharpEngine-csharp-swig)
        ##add_dependencies(NeoSharpNativeEngine build-neo-sharp-engine)

        add_dependencies(NeoSharpEngineCLI NeoSharpEngine)

        ## FIXME: Put at one point!
        install(TARGETS NeoSharpEngine DESTINATION ${NEO_INSTALL_DIR}/templates/NeoSharpTemplate)
        install(FILES ${NEO_BINARY_OUTPUT}/NeoSharpEngine.dll DESTINATION ${NEO_INSTALL_DIR}/templates/NeoSharpTemplate/MonoGame)

    endif (${CSHARP_FOUND})

    IF (${Java_FOUND} AND ${JNI_FOUND})

        set_source_files_properties(NeoEngineJava.i PROPERTIES CPLUSPLUS ON)
        set(CMAKE_SWIG_OUTDIR ${JAVA_OUTPUT})
        set(CMAKE_SWIG_FLAGS -DSWIG_JAVA -Doverride)
        set_property(SOURCE NeoEngineJava.i PROPERTY SWIG_MODULE_NAME Neo)

        swig_add_module(NeoEngineJava java NeoEngineJava.i)

        swig_link_libraries(NeoEngineJava ${NEO_ENGINE_LIBRARIES} ${NEO_CORE_LIBRARIES} ${JNI_LIBRARIES})
        target_include_directories(NeoEngineJava PUBLIC ${JNI_INCLUDE_DIRS} ${NEO_CORE_INCLUDE_DIR} ${NEO_ENGINE_INCLUDE_DIR})

        ## Execute during configuration so it can generate the Java files needed
        execute_process(
                COMMAND ${SWIG_EXECUTABLE} -Doverride -java -c++ ${CMAKE_SWIG_FLAGS} -outdir ${CMAKE_SWIG_OUTDIR}
                -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR}
                -o ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava_wrap.cxx -package NeoEngine -DSWIG_JAVA

                ${CMAKE_CURRENT_SOURCE_DIR}/NeoEngineJava.i
                OUTPUT_VARIABLE SWIG_swiglib_output
                ERROR_VARIABLE SWIG_swiglib_error
                RESULT_VARIABLE SWIG_swiglib_result
        )

        file(GLOB JAVA_SOURCES ${JAVA_OUTPUT}/*.java)
        add_jar(NeoEngineJar ${JAVA_SOURCES})
        add_dependencies(NeoEngineJar NeoEngineJava)

        install(TARGETS libNeoEngineJava DESTINATION ${NEO_INSTALL_DIR})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava.jar DESTINATION ${NEO_INSTALL_DIR})
    ENDIF (${Java_FOUND} AND ${JNI_FOUND})

    if (NOT ${SWIG_swiglib_result} EQUAL 0)
        message(SEND_ERROR "-- Generating script bindings for Neo failed with output:\n${SWIG_swiglib_error}")
    endif (NOT ${SWIG_swiglib_result} EQUAL 0)

    set_source_files_properties(NeoEngineLua.i PROPERTIES CPLUSPLUS ON)
    set(CMAKE_SWIG_FLAGS -Doverride)
    # set_property(SOURCE NeoEngineLua.i PROPERTY SWIG_MODULE_NAME NeoEngine)

    swig_add_module(NeoEngineLua lua NeoEngineLua.i)

    swig_link_libraries(NeoEngineLua ${NEO_ENGINE_LIBRARIES} ${NEO_CORE_LIBRARIES} ${LUA_LIBRARIES})
    target_include_directories(NeoEngineLua PUBLIC ${LUA_INCLUDE_DIR} ${NEO_CORE_INCLUDE_DIR} ${NEO_ENGINE_INCLUDE_DIR})

    #IF(NOT ANDROID AND NOT EMSCRIPTEN)
    ## Build from cache, even if the files could not be updated
    #ADD_LIBRARY(NeoEngineLua SHARED ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineLua_wrap.cxx)
    #TARGET_LINK_LIBRARIES(NeoEngineLua ${NEO_ENGINE_LIBRARIES} ${NEO_CORE_LIBRARIES} ${LUA_LIBRARIES})

    install(TARGETS NeoEngineLua DESTINATION ${NEO_INSTALL_DIR})
    #ENDIF()

ELSE ()
    MESSAGE("-- Will NOT generate script bindings for NeoEngine")
ENDIF ()
