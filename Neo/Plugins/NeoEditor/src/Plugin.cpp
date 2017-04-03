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
#include "ExternalToolset.h"

EditorGame g_editorGame(make_shared<ExternalToolset>());

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getGame()->registerSubGame(&g_editorGame);

	// Set flag to show the editor is running
	engine->getConfigurationRegistry().registerVariable("g_editor");
}

void EndPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getGame()->removeSubGame(&g_editorGame);

	// Remove flag to show the editor is not running anymore
	engine->getConfigurationRegistry().removeVariable("g_editor");
}