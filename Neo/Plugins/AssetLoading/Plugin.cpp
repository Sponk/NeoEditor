#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>

#include "AssimpMeshLoader.h"
#include "MeshLoad.h"
#include "ImageLoader.h"
#include "ImageSaver.h"
#include "SoundLoader.h"
#include "BinMeshLoader.h"
#include "BinFontLoader.h"
#include "LevelLoad.h"
#include "LevelSave.h"

using namespace Neo;

void StartPlugin()
{
	NeoEngine* engine = NeoEngine::getInstance();

	engine->getMeshLoader()->addLoader(xmlMeshLoad);
	engine->getMeshLoader()->addLoader(M_loadBinMesh);
	engine->getMeshLoader()->addLoader(M_loadAssimpMesh);

	engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
	engine->getArmatureAnimLoader()->addLoader(M_loadBinArmatureAnim);

	engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
	engine->getTexturesAnimLoader()->addLoader(M_loadBinTexturesAnim);

	engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);
	engine->getMaterialsAnimLoader()->addLoader(M_loadBinMaterialsAnim);

	engine->getImageLoader()->addLoader(M_loadImage);
	engine->getImageLoader()->addSaver(M_saveImage);

	engine->getLevelLoader()->addLoader(xmlLevelLoad);
	engine->getLevelLoader()->addSaver(M_saveLevel);
}

void EndPlugin()
{

}
