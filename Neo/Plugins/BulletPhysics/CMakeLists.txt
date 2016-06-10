
add_library(BulletPhysics SHARED Plugin.cpp BulletContext.cpp BulletContext.h)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${BULLET_INCLUDE_DIR}
)

target_link_libraries(BulletPhysics
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
		${BULLET_LIBRARIES}
)

install(TARGETS BulletPhysics DESTINATION ${NEO_INSTALL_DIR})
