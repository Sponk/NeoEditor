#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "PackageManagerNPK.h"

static PackageManagerNPK g_packageManager;

void StartPlugin()
{
	Neo::NeoEngine::getInstance()->setPackageManager(&g_packageManager);
	g_packageManager.init();
}

void EndPlugin()
{
	g_packageManager.cleanup();
}
