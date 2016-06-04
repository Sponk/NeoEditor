
add_library(AssetLoading SHARED
    Plugin.cpp
    AssimpMeshLoader.cpp
    BinFontLoader.cpp
    BinMeshLoader.cpp
    FreetypeLoader.cpp
    ImageLoader.cpp
    JpegLoader.cpp
    PngLoader.cpp
    SoundLoader.cpp
    TgaLoader.cpp
    VorbisLoader.cpp
    WAVLoader.cpp
    LevelLoad.cpp
    MeshLoad.cpp

	LispLevelLoad.cpp
	LispLevelSave.cpp

    LevelSave.cpp
    MeshSave.cpp
    ImageSaver.cpp
    PngSaver.cpp

    XmlCommon.cpp)

include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${NEO_CORE_INCLUDE_DIR}
		${NEO_ENGINE_INCLUDE_DIR}
		${ASSIMP_INCLUDE_DIR}
		${FREETYPE_INCLUDE_DIR}
		${VORBIS_INCLUDE_DIR}
		${PNG_INCLUDE_DIR}
		${JPEG_INCLUDE_DIR}
		${OGG_INCLUDE_DIR}
		${TINYXML_INCLUDE_DIR}
		${ZLIB_INCLUDE_DIR}
		${SEXPRESSO_INCLUDE_DIR}
		${GTEST_INCLUDE_DIR}
)

target_link_libraries(AssetLoading
		${NEO_CORE_LIBRARIES}
		${NEO_ENGINE_LIBRARIES}

		${ASSIMP_LIBRARIES}
		${FREETYPE_LIBRARIES}
		${VORBIS_LIBRARIES}
		${PNG_LIBRARIES}
		${JPEG_LIBRARIES}
		${OGG_LIBRARIES}
		${TINYXML_LIBRARIES}
		${ZLIB_LIBRARIES}
		${SEXPRESSO_LIBRARIES}
)

if(NOT NO_TESTS)
	add_executable(AssetLoadingTest tests/main.cpp LispLevelLoad.cpp LispLevelSave.cpp tests/LispLevelTest.cpp)
	target_link_libraries(AssetLoadingTest ${SEXPRESSO_LIBRARIES} ${NEO_CORE_LIBRARIES} ${NEO_ENGINE_LIBRARIES} ${GTEST_LIBRARIES})
endif()

install(TARGETS AssetLoading DESTINATION ${NEO_INSTALL_DIR})
