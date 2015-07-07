
SET(CMAKE_SHARED_LIBRARY_PREFIX "")
INCLUDE_DIRECTORIES(
    ${NEO_CORE_INCLUDE_DIR}
    ${NEO_ENGINE_INCLUDE_DIR}
    ${NEO2D_INCLUDE_DIR}
    ${SDL_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${MONO_INCLUDE_DIR}
)

IF(${SWIG_FOUND})
	MESSAGE("-- Generating Lua bindings for Neo")

  	SET(CMAKE_SHARED_LIBRARY_PREFIX "")
	EXECUTE_PROCESS(
		COMMAND ${SWIG_EXECUTABLE} -lua -c++ -I${NEO_CORE_INCLUDE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/Includes
                        -I${NEO2D_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/NeoLua.i

		OUTPUT_VARIABLE SWIG_swiglib_output
		ERROR_VARIABLE SWIG_swiglib_error
		RESULT_VARIABLE SWIG_swiglib_result
		)

	ADD_CUSTOM_TARGET(neo-swig
                COMMAND ${SWIG_EXECUTABLE} -lua -c++ -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR} -I${NEO2D_INCLUDE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/NeoLua.i
		)

        IF(${ENABLE_MONO})

  		FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Swig)

			## Select library to import
			IF(WIN32)
				SET(DLLIMPORT -dllimport Game.dll)
			ELSE()
				SET(DLLIMPORT -dllimport Game.so)
			ENDIF()

	  	EXECUTE_PROCESS(
	  		COMMAND ${SWIG_EXECUTABLE} -csharp -c++ -outdir ${CMAKE_BINARY_DIR}/Swig -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR}
                                -I${NEO2D_INCLUDE_DIR} ${DLLIMPORT} ${CMAKE_CURRENT_SOURCE_DIR}/NeoSharp.i

			OUTPUT_VARIABLE SWIG_swiglib_output
			ERROR_VARIABLE SWIG_swiglib_error
			RESULT_VARIABLE SWIG_swiglib_result
                )

	  	ADD_CUSTOM_TARGET(neo-mono-swig
			COMMAND ${SWIG_EXECUTABLE} -csharp -c++ -outdir ${CMAKE_BINARY_DIR}/Swig -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR}
                                -I${NEO2D_INCLUDE_DIR} ${DLLIMPORT} ${CMAKE_CURRENT_SOURCE_DIR}/NeoSharp.i
			)

		FILE(GLOB_RECURSE SWIG_SOURCES ${CMAKE_BINARY_DIR}/Swig/*.cs)
                FILE(GLOB_RECURSE PLUGIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/NeoSharp/*.cpp)

		ADD_CUSTOM_TARGET(build-neo-sharp ALL ${MCS_EXECUTABLE} -t:library -out:${NEO_BINARY_OUTPUT}/NeoSharp.dll ${SWIG_SOURCES})

		IF(NOT ANDROID AND NOT EMSCRIPTEN)
                        ADD_LIBRARY(Game SHARED ${PLUGIN_SOURCES} NeoSharp_wrap.cxx)
		ELSE()
                        ADD_LIBRARY(Game SHARED ${PLUGIN_SOURCES} NeoSharp_wrap.cxx)
		ENDIF()

                TARGET_LINK_LIBRARIES(Game ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES} ${NEO2D_LIBRARIES} ${MONO_LIBRARIES} ${LUA_LIBRARIES})

                ## Install C# plugin
                INSTALL(TARGETS Game DESTINATION ${NEO_INSTALL_DIR})
                INSTALL(FILES ${NEO_BINARY_OUTPUT}/NeoSharp.dll DESTINATION ${NEO_INSTALL_DIR})
  	ENDIF()
  IF(NOT ${SWIG_swiglib_result} EQUAL 0)
	MESSAGE(SEND_ERROR "-- Generating Lua bindings for Neo failed with output:\n${SWIG_swiglib_error}")
  ENDIF(NOT ${SWIG_swiglib_result} EQUAL 0)
ELSE()
  MESSAGE("-- Will NOT generate Lua bindings for Neo")
ENDIF()

## Build from cache, even if the files could not be updated
ADD_LIBRARY(NeoLua SHARED NeoLua_wrap.cxx)
TARGET_LINK_LIBRARIES(NeoLua ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES} ${NEO2D_LIBRARIES} ${LUA_LIBRARIES})

## Install Lua API
INSTALL(TARGETS NeoLua DESTINATION ${NEO_INSTALL_DIR})
