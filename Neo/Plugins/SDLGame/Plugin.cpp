#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "SDLGame.h"
#include "SDLInputContext.h"

#ifndef EMSCRIPTEN
#include "SDLThread.h"
#endif

#include "SDLSystem.h"

static SDLGame g_sdlGame;
static SDLInputContext g_sdlInputContext;
static SDLSystem g_sdlSystemContext;

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->setGame(&g_sdlGame);
	engine->setInputContext(&g_sdlInputContext);
	engine->setSystemContext(&g_sdlSystemContext);

#ifndef EMSCRIPTEN
	Neo::ThreadFactory::getInstance()->setTemplateSemaphore(new SDLSemaphore);
	Neo::ThreadFactory::getInstance()->setTemplateThread(new SDLThread);
#endif
}

void EndPlugin()
{
	Neo::NeoEngine::getInstance()->setGame(NULL);
}
