
SET(CMAKE_SHARED_LIBRARY_PREFIX "")

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

include(Swig)

IF(${SWIG_FOUND})
  MESSAGE("-- Generating script bindings for NeoCore")

  SET(CMAKE_SHARED_LIBRARY_PREFIX "")

  if(${CSHARP_FOUND})
	  if(WIN32)
		  set(DLLIMPORT -dllimport NeoSharpNativeCore.dll)
	  else()
		  set(DLLIMPORT -dllimport NeoSharpNativeCore.so)
	  endif()

	  set(SWIG_ARGS -I${NEO_CORE_INCLUDE_DIR} ${DLLIMPORT})
	  add_swig_module(NeoSharpCore csharp NeoSharpCore.i "${SWIG_ARGS}")
	  add_custom_target(build-neo-sharp-core ALL ${CSHARP_COMPILER} -t:library -out:${NEO_BINARY_OUTPUT}/NeoSharpCore.dll ${NeoSharpCore_OUTDIR}/*.cs)

	  add_library(NeoSharpNativeCore SHARED ${NeoSharpCore_SOURCES})

	  include_directories(${NEO_CORE_INCLUDE_DIR} ${NEO_ENGINE_INCLUDE_DIR} ${MONO_INCLUDE_DIRS})

	  target_link_libraries(NeoSharpNativeCore
			  ${NEO_CORE_LIBRARIES}
			  ${NEO_ENGINE_LIBRARIES}
			  ${MONO_LIBRARIES}
			  )

	  add_dependencies(NeoSharpNativeCore build-neo-sharp-core)
	  add_dependencies(build-neo-sharp-core NeoSharpCore-csharp-swig)

  endif(${CSHARP_FOUND})

  ## Generate NeoCore plugin
  EXECUTE_PROCESS(
	COMMAND ${SWIG_EXECUTABLE} -lua -c++ -includeall -ignoremissing -I${NEO_CORE_INCLUDE_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/NeoCoreLua_wrap.cxx
		  ${CMAKE_CURRENT_SOURCE_DIR}/NeoCoreLua.i
	
	OUTPUT_VARIABLE SWIG_swiglib_output
	ERROR_VARIABLE SWIG_swiglib_error
	RESULT_VARIABLE SWIG_swiglib_result
	)

  ADD_CUSTOM_TARGET(neo-core-lua-swig
    COMMAND ${SWIG_EXECUTABLE} -lua -includeall -ignoremissing -c++ -I${NEO_CORE_INCLUDE_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/NeoCoreLua_wrap.cxx
		  ${CMAKE_CURRENT_SOURCE_DIR}/NeoCoreLua.i
	)

  IF(${Java_FOUND} AND ${JNI_FOUND})
	  EXECUTE_PROCESS(
			  COMMAND ${SWIG_EXECUTABLE} -java -c++ -includeall -ignoremissing -outdir ${JAVA_OUTPUT} -I${NEO_CORE_INCLUDE_DIR}
			  -o ${CMAKE_CURRENT_BINARY_DIR}/NeoCoreJava_wrap.cxx -package NeoCore -DSWIG_JAVA
			  ${CMAKE_CURRENT_SOURCE_DIR}/NeoCoreJava.i

			  OUTPUT_VARIABLE SWIG_swiglib_output
			  ERROR_VARIABLE SWIG_swiglib_error
			  RESULT_VARIABLE SWIG_swiglib_result
	  )

	  ADD_CUSTOM_TARGET(neo-core-java-swig
			  COMMAND ${SWIG_EXECUTABLE} -java -includeall -ignoremissing -c++ -I${NEO_CORE_INCLUDE_DIR} -outdir ${JAVA_OUTPUT}
			  -o ${CMAKE_CURRENT_BINARY_DIR}/NeoCoreJava_wrap.cxx -package NeoCore -DSWIG_JAVA
			  ${CMAKE_CURRENT_SOURCE_DIR}/NeoCoreJava.i
			  )

	  ADD_LIBRARY(libNeoCoreJava SHARED ${CMAKE_CURRENT_BINARY_DIR}/NeoCoreJava_wrap.cxx)
	  TARGET_INCLUDE_DIRECTORIES(libNeoCoreJava PUBLIC ${JNI_INCLUDE_DIRS})
	  TARGET_LINK_LIBRARIES(libNeoCoreJava ${NEO_CORE_LIBRARIES} ${JNI_LIBRARIES})

	  FILE(GLOB JAVA_SOURCES ${JAVA_OUTPUT}/*.java)
	  ADD_JAR(NeoCoreJava ${JAVA_SOURCES})

	  add_dependencies(NeoCoreJava neo-core-java-swig)
	  add_dependencies(libNeoCoreJava neo-core-java-swig)

	  INSTALL(TARGETS libNeoCoreJava DESTINATION ${NEO_INSTALL_DIR})
	  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/NeoCoreJava.jar DESTINATION ${NEO_INSTALL_DIR})

  ENDIF(${Java_FOUND} AND ${JNI_FOUND})

  IF(NOT ${SWIG_swiglib_result} EQUAL 0)
	MESSAGE(SEND_ERROR "-- Generating script bindings for Neo failed with output:\n${SWIG_swiglib_error}")
  ENDIF(NOT ${SWIG_swiglib_result} EQUAL 0)

  #IF(NOT ANDROID AND NOT EMSCRIPTEN)
	  ## Build from cache, even if the files could not be updated
	  ADD_LIBRARY(NeoCoreLua SHARED ${CMAKE_CURRENT_BINARY_DIR}/NeoCoreLua_wrap.cxx)
	  TARGET_LINK_LIBRARIES(NeoCoreLua ${NEO_CORE_LIBRARIES} ${LUA_LIBRARIES})

	  ## Install Lua API
	  INSTALL(TARGETS NeoCoreLua DESTINATION ${NEO_INSTALL_DIR})
  #ENDIF()

ELSE()
  MESSAGE("-- Will NOT generate script bindings for NeoCore")
ENDIF()
