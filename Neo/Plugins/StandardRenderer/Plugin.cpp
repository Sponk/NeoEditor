#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "StandardRenderer.h"

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getRendererManager()->addRenderer("StandardRenderer", Neo::StandardRenderer::getNew);
	engine->setRenderer(new Neo::StandardRenderer());
}

void EndPlugin()
{

}
