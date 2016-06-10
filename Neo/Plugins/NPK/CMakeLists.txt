
add_library(NPK SHARED Plugin.cpp PackageManagerNPK.cpp PackageManagerNPK.h)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${NPK_INCLUDE_DIR}
)

target_link_libraries(NPK
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${NPK_LIBRARIES}
)

install(TARGETS NPK DESTINATION ${NEO_INSTALL_DIR})
