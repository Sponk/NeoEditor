#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "MonoSubgame.h"
#include "MonoContext.h"

using namespace Neo;

MonoSubgame* sg = new MonoSubgame();

extern "C" void StartPlugin()
{
	NeoEngine* engine = NeoEngine::getInstance();
	NeoGame* game = engine->getGame();
	MonoContext* monoContext = MonoContext::getInstance();

	const char* enableDebugStr = getenv("NEOSHARP_DEBUG");
	bool enableDebug = (enableDebugStr && strcmp(enableDebugStr, "true") == 0);

	if(enableDebug) MLOG_INFO("Enabled Mono soft debugger");

	// Enable debugging
	monoContext->enableDebugging(enableDebug);

	// Load our assembly and init subgame if
	// it is successfully loaded
	if(monoContext->loadAssembly("NeoSharpEngine.dll")
		&& monoContext->addAssembly("MonoGame.dll"))
	{
		game->registerSubGame(sg);
	}

	// Load external assembly with game specific code
	//monoContext->callMethod("NeoGame:loadExternalAssembly()");
}

extern "C" void EndPlugin()
{
	SAFE_DELETE(sg);
}
