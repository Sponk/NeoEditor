#-------------------------------------------------------------------
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
# Try to find CoreMotion
# Once done this will define
#
#   COREMOTION_FOUND
#   COREMOTION_LIBRARIES

IF(APPLE)
  create_search_paths(/Developer/Platforms)
  findpkg_framework(CoreMotion)
  SET(COREMOTION_LIBRARY "-framework CoreMotion")
ENDIF(APPLE)

SET(COREMOTION_FOUND "NO")
IF(COREMOTION_LIBRARY)
  SET(COREMOTION_LIBRARIES "${COREMOTION_LIBRARY}")
  SET(COREMOTION_FOUND "YES")
ENDIF(COREMOTION_LIBRARY)
