#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "BulletContext.h"

static BulletContext g_bulletContext;

void StartPlugin()
{
	Neo::NeoEngine::getInstance()->setPhysicsContext(&g_bulletContext);
}

void EndPlugin()
{

}