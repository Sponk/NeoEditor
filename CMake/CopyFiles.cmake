macro(copy_files GLOBPAT DESTINATION TARGETNAME)
  file(GLOB_RECURSE COPY_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLOBPAT})
  add_custom_target(${TARGETNAME} ALL
    COMMENT "Copying files: ${GLOBPAT}")

  foreach(FILENAME ${COPY_FILES})
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
    set(DST "${DESTINATION}/${FILENAME}")

    if(NOT IS_DIRECTORY ${SRC})
    add_custom_command(
      TARGET ${TARGETNAME}
      COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
      )
    endif()
  endforeach(FILENAME)
endmacro(copy_files)