#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "LuaScript.h"
#include "LuaBehavior.h"

static LuaScript g_scriptContext;

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	Neo::BehaviorManager* manager = engine->getBehaviorManager();

	engine->setScriptContext(&g_scriptContext);
	manager->addBehavior(Neo::LuaBehavior::getStaticName(), OBJECT3D, Neo::LuaBehavior::getNew);

	// Output some info about the middleware used
#ifndef LUAJIT_VERSION
	MLOG_INFO("Lua interpreter:\t" << LUA_VERSION);
	MLOG_INFO("Lua credits:\t" << LUA_AUTHORS);
#else
	MLOG_INFO("Lua interpreter:\t" << LUAJIT_VERSION);
	MLOG_INFO("Lua credits:\t" << LUAJIT_COPYRIGHT);
#endif
}

void EndPlugin()
{

}
