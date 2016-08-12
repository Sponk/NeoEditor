#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "GLESRenderer.h"

static Neo::GLESRenderer g_renderer;

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getRendererManager()->addRenderer(Neo::GLESRenderer::getStaticName(), Neo::GLESRenderer::getNew);
	engine->setRenderer(&g_renderer);
}

void EndPlugin()
{

}
