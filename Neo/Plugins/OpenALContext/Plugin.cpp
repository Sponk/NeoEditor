#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "ALContext.h"

static Neo::ALContext* g_alContext;

void StartPlugin()
{
	g_alContext = new Neo::ALContext();
	Neo::NeoEngine::getInstance()->setSoundContext(g_alContext);
}

void EndPlugin()
{
	delete g_alContext;
}
