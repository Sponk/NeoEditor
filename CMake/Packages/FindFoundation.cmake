#-------------------------------------------------------------------
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
# Try to find Foundation
# Once done this will define
#
#   FOUNDATION_FOUND
#   FOUNDATION_LIBRARIES

IF(APPLE)
  create_search_paths(/Developer/Platforms)
  findpkg_framework(Foundation)
  SET(COREGRAPHICS_LIBRARY "-framework Foundation")
ENDIF(APPLE)

SET(FOUNDATION_FOUND "NO")
IF(FOUNDATION_LIBRARY)
  SET(FOUNDATION_LIBRARIES "${FOUNDATION_LIBRARY}")
  SET(FOUNDATION_FOUND "YES")
ENDIF(FOUNDATION_LIBRARY)
