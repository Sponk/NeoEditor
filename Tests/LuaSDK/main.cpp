//========================================================================
// Copyright (c) 2014-2015 Yannick Pflanzer <neo-engine.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <NeoEngine.h>
#include <LuaBehavior.h>
#include <WinContext.h>
#include <PackageManagerNPK.h>
#include <GuiSystem.h>
#include <SDLThread.h>
#include <Server.h>
#include <BulletContext.h>

#include <gtest/gtest.h>
#include <NeoCore.h>
#include <Window/Window.h>

#ifdef main
#undef main
#endif

using namespace Neo;
using namespace Gui;

bool compare_float(float a, float b)
{
	return fabs(a - b) < 0.0001;
}

class TestNeoSDK
{
public:

	static void SetUp()
	{
		NeoEngine* engine = NeoEngine::getInstance();

		NeoGame* game = new NeoGame();

		// Setup script context
		Neo::LuaScript* script = new Neo::LuaScript();
		
		Level* level = new Level();
		SystemContext* context = (SystemContext*) new Neo::MWinContext();
		PackageManager* pmanager = new Neo::PackageManagerNPK;
		Neo::BulletContext* physcontext = new Neo::BulletContext();

		pmanager->init();

		level->addNewScene();
		level->setCurrentSceneId(0);

		engine->setLevel(level);
		engine->setScriptContext(script);
		engine->setSystemContext(context);
		engine->setGame(game);
		engine->setPackageManager(pmanager);
		engine->setPhysicsContext(physcontext);

		engine->getBehaviorManager()->addBehavior(Neo::LuaBehavior::getStaticName(), M_OBJECT3D, Neo::LuaBehavior::getNew);

		// Init default thread
		ThreadFactory* mgr = ThreadFactory::getInstance();
		mgr->setTemplateSemaphore(new SDLSemaphore());
		mgr->setTemplateThread(new SDLThread());

		NeoWindow::getInstance()->createSemaphores();

		Gui::GuiSystem::getInstance()->setupLuaInterface(script);
		game->begin();
	}

	static void TearDown()
	{
		//MLOG_INFO("Clearing engine");
		NeoEngine* engine = NeoEngine::getInstance();

		delete engine->getSystemContext();
		delete engine->getGame();
		delete engine->getLevel();
		delete engine->getScriptContext();
		delete engine->getPackageManager();

		ThreadFactory::getInstance()->clear();

		engine->setLevel(NULL);
		engine->setScriptContext(NULL);
		engine->setSystemContext(NULL);
		engine->setGame(NULL);
		engine->setPackageManager(NULL);
	}
};

// Run LuaUnit based tests.
const char* lua_code =  "enableNeo3DTests = true\n"
                        "dofile(\"scripts/SDK/Neo3D.lua\")\n"
                        "Neo3D.runTests()";


int main(int argc, char* argv[])
{
	TestNeoSDK::SetUp();

	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	script->runString(lua_code);

	TestNeoSDK::TearDown();
	return 0;
}
