#-------------------------------------------------------------------
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
# Try to find CoreGraphics
# Once done this will define
#
#   COREGRAPHICS_FOUND
#   COREGRAPHICS_LIBRARIES

IF(APPLE)
  create_search_paths(/Developer/Platforms)
  findpkg_framework(CoreGraphics)
  SET(COREGRAPHICS_LIBRARY "-framework CoreGraphics")
ENDIF(APPLE)

SET(COREGRAPHICS_FOUND "NO")
IF(COREGRAPHICS_LIBRARY)
  SET(COREGRAPHICS_LIBRARIES "${COREGAPHICS_LIBRARY}")
  SET(COREGRAPHICS_FOUND "YES")
ENDIF(COREGRAPHICS_LIBRARY)
