#-------------------------------------------------------------------
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
# Try to find UIKit
# Once done this will define
#
#   UIKIT_FOUND
#   UIKIT_LIBRARIES

IF(APPLE)
  create_search_paths(/Developer/Platforms)
  findpkg_framework(UIKit)
  SET(COREGRAPHICS_LIBRARY "-framework UIKit")
ENDIF(APPLE)

SET(UIKIT_FOUND "NO")
IF(UIKIT_LIBRARY)
  SET(UIKIT_LIBRARIES "${UIKIT_LIBRARY}")
  SET(UIKIT_FOUND "YES")
ENDIF(UIKIT_LIBRARY)
