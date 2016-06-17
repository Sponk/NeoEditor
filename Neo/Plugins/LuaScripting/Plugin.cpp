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

static LuaScript* g_scriptContext;

class LuaSubgame : public Neo::SubGame
{
public:
    void onBegin()
    {
        // g_scriptContext.runScript("assets/main.lua");
    }

    void update()
    {
        PROFILE_BEGIN("ScriptUpdate");
        // update script
        if(g_scriptContext->startCallFunction("update"))
        {
            g_scriptContext->pushFloat(Neo::NeoEngine::getInstance()->getGame()->getFrameDelta());
            g_scriptContext->endCallFunction(1);
        }
        PROFILE_END("ScriptUpdate");
    }

    void draw()
    {
        PROFILE_BEGIN("ScriptDraw");
        g_scriptContext->callFunction("draw");
        PROFILE_END("ScriptDraw");
    }

    void onEnd()
    {
        g_scriptContext->callFunction("onEnd");
    }
} g_luaSubgame;

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	Neo::BehaviorManager* manager = engine->getBehaviorManager();

	g_scriptContext = new LuaScript;
	engine->setScriptContext(g_scriptContext);
	manager->addBehavior(Neo::LuaBehavior::getStaticName(), OBJECT3D, Neo::LuaBehavior::getNew);

    engine->getGame()->registerSubGame(&g_luaSubgame);

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
	delete g_scriptContext;
}
