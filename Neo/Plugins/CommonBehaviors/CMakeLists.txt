
add_library(CommonBehaviors SHARED
    Plugin.cpp
    FollowBehavior.cpp
    LookAtBehavior.cpp
    ParticleSystemBehavior.cpp)

include_directories(
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
)

target_link_libraries(CommonBehaviors
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}
)

install(TARGETS CommonBehaviors DESTINATION ${NEO_INSTALL_DIR})
