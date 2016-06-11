
macro(add_swig_module name type input includes)
    MESSAGE("-- Generating " ${type} " bindings for " ${name})

    set(SWIG_SOURCES  ${CMAKE_CURRENT_BINARY_DIR}/${name}_${type}_wrap.cxx)
    set(${name}_SOURCES ${SWIG_SOURCES})
    set(SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/${name}_${type})
    set(${name}_OUTDIR ${SWIG_OUTDIR})
    file(MAKE_DIRECTORY ${SWIG_OUTDIR})

    EXECUTE_PROCESS(
            COMMAND ${SWIG_EXECUTABLE} -${type} -c++ -includeall -ignoremissing "${includes}" -o ${SWIG_SOURCES}
            -outdir ${SWIG_OUTDIR} ${CMAKE_CURRENT_SOURCE_DIR}/${input}

            OUTPUT_VARIABLE SWIG_swiglib_output
            ERROR_VARIABLE SWIG_swiglib_error
            RESULT_VARIABLE SWIG_swiglib_result
    )

    ADD_CUSTOM_TARGET(${name}-${type}-swig
            COMMAND ${SWIG_EXECUTABLE} -${type} -includeall -ignoremissing -c++ ${includes} -o ${SWIG_SOURCES}
            -outdir ${SWIG_OUTDIR} ${CMAKE_CURRENT_SOURCE_DIR}/${input}
            )
endmacro(add_swig_module)