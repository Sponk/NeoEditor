#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "EditorGame.h"
#include "ExternalQtToolset.h"

EditorGame g_editorGame(make_shared<ExternalQtToolset>());

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getGame()->registerSubGame(&g_editorGame);
}

void EndPlugin()
{

}