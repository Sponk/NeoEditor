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
#include <MCore.h>
#include <LuaBehavior.h>
#include <WinContext.h>
#include <PackageManagerNPK.h>
#include <GuiSystem.h>
#include <SDLThread.h>
#include <Server.h>
#include <BulletContext.h>

#include <gtest/gtest.h>
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
		Neo::LuaScript* script = new Neo::LuaScript();
		Level* level = new Level();
		MSystemContext* context = (MSystemContext*) new Neo::MWinContext();
		MPackageManager* pmanager = new Neo::MPackageManagerNPK;
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
		MThreadManager* mgr = MThreadManager::getInstance();
		mgr->setTemplateSemaphore(new SDLSemaphore());
		mgr->setTemplateThread(new SDLThread());

		NeoWindow::getInstance()->createSemaphores();

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

		MThreadManager::getInstance()->clear();

		engine->setLevel(NULL);
		engine->setScriptContext(NULL);
		engine->setSystemContext(NULL);
		engine->setGame(NULL);
		engine->setPackageManager(NULL);
	}
};


// This is not really a unit test since it tests the whole networking
// system at once.
/*bool testFunctionSuccess = false;
void testFunction(RakNet::BitStream* in)
{
    NeoVariable arg1 = readNextArgument(in);

    if(arg1.getPointer() && arg1.getType() == M_VARIABLE_STRING)
    {
        MLOG_INFO("Got argument: " << ((MString*)arg1.getPointer())->getSafeString());

        if(!strcmp(((MString*)arg1.getPointer())->getSafeString(), "Hello World!"))
           testFunctionSuccess = true;
    }
}

LT_BEGIN_AUTO_TEST(NeoTestSdk, Networking_test);

	NeoGame* game = NeoEngine::getInstance()->getGame();
	MWindow* window = MWindow::getInstance();

	game->begin();

	Server server;
	Server client;

	server.addRPCFunction("testFunction", testFunction);

    server.startServer(10, 60001);
    client.startClient("127.0.0.1", 60001);

    window->sleep(1000);

	std::vector<NeoVariable> variables;
    variables.push_back(NeoVariable("Test", new MString("Hello World!"), M_VARIABLE_STRING));

	Messenger::getInstance()->sendMessage("testFunction", ID_RPC_MESSAGE, &variables, "ClientThread", "MainThread");

    window->sleep(100);
    LT_CHECK(testFunctionSuccess);

    Messenger::getInstance()->sendMessage("testFunction", ID_RPC_MESSAGE, &variables, "ClientThread", "MainThread");

    window->sleep(100);

    LT_CHECK(testFunctionSuccess);

    delete (MString*) variables[0].getPointer();
    variables.clear();

	game->end();

LT_END_AUTO_TEST(Networking_test);*/

// Run LuaUnit based tests.
// FIXME: Should be in another executable!
const char* lua_code =  "enableNeo3DTests = true\n"
                        "dofile(\"scripts/SDK/Neo3D.lua\")\n"
                        "Neo3D.runTests()";


int main(int argc, char* argv[])
{
	TestNeoSDK::SetUp();

	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	script->runString(lua_code);

	TestNeoSDK::TearDown();
	return 0;
}
