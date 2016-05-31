
SET(CMAKE_SHARED_LIBRARY_PREFIX "")

find_package(JNI)
find_package(Java)
include(UseJava)

if(${Java_FOUND} STREQUAL "")
set(Java_FOUND FALSE)
endif()

INCLUDE_DIRECTORIES(
  ${NEO_CORE_INCLUDE_DIR}
  ${NEO_ENGINE_INCLUDE_DIR}
  ${SDL_INCLUDE_DIR}
  ${LUA_INCLUDE_DIR}
  ${MONO_INCLUDE_DIR}
)

set(JAVA_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/java)

file(REMOVE ${JAVA_OUTPUT})
file(MAKE_DIRECTORY ${JAVA_OUTPUT})

IF(${SWIG_FOUND})
  MESSAGE("-- Generating script bindings for NeoEngine")

  SET(CMAKE_SHARED_LIBRARY_PREFIX "")

  EXECUTE_PROCESS(
	COMMAND ${SWIG_EXECUTABLE} -lua -c++ -includeall -ignoremissing -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineLua_wrap.cxx
		  ${CMAKE_CURRENT_SOURCE_DIR}/NeoEngineLua.i
	
	OUTPUT_VARIABLE SWIG_swiglib_output
	ERROR_VARIABLE SWIG_swiglib_error
	RESULT_VARIABLE SWIG_swiglib_result
	)

  ADD_CUSTOM_TARGET(neo-engine-lua-swig ALL
    COMMAND ${SWIG_EXECUTABLE} -lua -includeall -ignoremissing -c++ -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineLua_wrap.cxx
		  ${CMAKE_CURRENT_SOURCE_DIR}/NeoEngineLua.i
	)

  IF(${Java_FOUND} AND ${JNI_FOUND})
	  EXECUTE_PROCESS(
			  COMMAND ${SWIG_EXECUTABLE} -java -c++ -includeall -ignoremissing -outdir ${JAVA_OUTPUT} -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR}
			  -o ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava_wrap.cxx -package NeoEngine -DSWIG_JAVA
			  ${CMAKE_CURRENT_SOURCE_DIR}/NeoEngineJava.i
			  OUTPUT_VARIABLE SWIG_swiglib_output
			  ERROR_VARIABLE SWIG_swiglib_error
			  RESULT_VARIABLE SWIG_swiglib_result
	  )

	  ADD_CUSTOM_TARGET(neo-engine-java-swig ALL
			  COMMAND ${SWIG_EXECUTABLE} -java -includeall -ignoremissing -c++ -I${NEO_CORE_INCLUDE_DIR} -I${NEO_ENGINE_INCLUDE_DIR} -outdir ${JAVA_OUTPUT}
			  -o ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava_wrap.cxx -package NeoEngine -DSWIG_JAVA
			  ${CMAKE_CURRENT_SOURCE_DIR}/NeoEngineJava.i
			  )

	  ADD_LIBRARY(libNeoEngineJava SHARED ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava_wrap.cxx)
	  TARGET_INCLUDE_DIRECTORIES(libNeoEngineJava PUBLIC ${JNI_INCLUDE_DIRS})
	  TARGET_LINK_LIBRARIES(libNeoEngineJava ${NEO_ENGINE_LIBRARIES} ${NEO_CORE_LIBRARIES} ${JNI_LIBRARIES})

	  FILE(GLOB JAVA_SOURCES ${JAVA_OUTPUT}/*.java)
	  ADD_JAR(NeoEngineJava ${JAVA_SOURCES})

	  add_dependencies(NeoEngineJava neo-core-java-swig)
	  add_dependencies(libNeoEngineJava neo-core-java-swig)

	  INSTALL(TARGETS libNeoEngineJava DESTINATION ${NEO_INSTALL_DIR})
	  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineJava.jar DESTINATION ${NEO_INSTALL_DIR})

  ENDIF(${Java_FOUND} AND ${JNI_FOUND})

  IF(NOT ${SWIG_swiglib_result} EQUAL 0)
	MESSAGE(SEND_ERROR "-- Generating script bindings for Neo failed with output:\n${SWIG_swiglib_error}")
  ENDIF(NOT ${SWIG_swiglib_result} EQUAL 0)

  #IF(NOT ANDROID AND NOT EMSCRIPTEN)
	  ## Build from cache, even if the files could not be updated
	  ADD_LIBRARY(NeoEngineLua SHARED ${CMAKE_CURRENT_BINARY_DIR}/NeoEngineLua_wrap.cxx)
	  TARGET_LINK_LIBRARIES(NeoEngineLua ${NEO_ENGINE_LIBRARIES} ${NEO_CORE_LIBRARIES} ${LUA_LIBRARIES})

	  ## Install Lua API
	  INSTALL(TARGETS NeoEngineLua DESTINATION ${NEO_INSTALL_DIR})
  #ENDIF()

ELSE()
  MESSAGE("-- Will NOT generate script bindings for NeoEngine")
ENDIF()
