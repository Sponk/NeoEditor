#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "ES2Context.h"

static Neo::ES2Context g_glContext;

void StartPlugin()
{
	Neo::NeoEngine::getInstance()->setRenderingContext(&g_glContext);
}

void EndPlugin()
{

}