#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "MobileRenderer.h"

static Neo::MobileRenderer* g_renderer;

void StartPlugin()
{	
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getRendererManager()->addRenderer("MobileRenderer", Neo::MobileRenderer::getNew);
	engine->setRenderer(g_renderer = new Neo::MobileRenderer());
}

void EndPlugin()
{
	delete g_renderer;
}
