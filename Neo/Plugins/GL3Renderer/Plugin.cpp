#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "GL3Renderer.h"

static Neo::GL3Renderer g_renderer;

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getRendererManager()->addRenderer(Neo::GL3Renderer::getStaticName(), Neo::GL3Renderer::getNew);
	engine->setRenderer(&g_renderer);

	engine->getConfigurationRegistry().setVariable("g_rendererType", "compatibility");
}

void EndPlugin()
{

}
