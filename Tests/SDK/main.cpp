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
#include <MWindow.h>
#include <WinContext.h>
#include <PackageManagerNPK.h>
#include <GuiSystem.h>
#include <SDLThread.h>
#include <Server.h>
#include <NetworkUtils.h>
#include <BulletContext.h>
#include "liblittletest.hpp"

#ifdef main
#undef main
#endif

using namespace Neo;
using namespace Gui;

bool compare_float(float a, float b)
{
	return fabs(a - b) < 0.0001;
}

LT_BEGIN_SUITE(NeoTestSdk);

void set_up()
{
	//MLOG_INFO("Setting up engine");
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

	MWindow::getInstance()->createSemaphores();

	game->begin();
}

void tear_down()
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

LT_END_SUITE(NeoTestSdk);

LT_BEGIN_AUTO_TEST(NeoTestSdk, MVector3_test);

	MVector3 testvec(1.0,1.0,1.0);
	testvec = testvec + testvec;
	LT_ASSERT(testvec == MVector3(2.0,2.0,2.0));

LT_END_AUTO_TEST(MVector3_test);

LT_BEGIN_AUTO_TEST(NeoTestSdk, LuaBehavior_test);

	NeoEngine* engine = NeoEngine::getInstance();

	Object3d* object = engine->getLevel()->getCurrentScene()->addNewLight();
	Neo::LuaBehavior* b = (Neo::LuaBehavior*) engine->getBehaviorManager()->getBehaviorByName("LuaScript")->getNewBehavior(object);
	NeoVariable var = b->getVariable(0);

	LT_ASSERT(var.getType() == M_VARIABLE_STRING);

	((MString*) var.getPointer())->set("test.lua");
	int num = b->getVariablesNumber();

	LT_ASSERT_EQ(num, 3);

	var = b->getVariable(0);
	LT_ASSERT_EQ(var.getType(), M_VARIABLE_STRING);

    var = b->getVariable(1);
	LT_ASSERT_EQ(var.getType(), M_VARIABLE_FLOAT);

	LT_ASSERT(compare_float(*((float*)var.getPointer()), 123.123));

LT_END_AUTO_TEST(LuaBehavior_test);

LT_BEGIN_AUTO_TEST(NeoTestSdk, LuaBehaviorNonExistantScript_test);

	NeoEngine* engine = NeoEngine::getInstance();

	Object3d* object = engine->getLevel()->getCurrentScene()->addNewLight();
	Neo::LuaBehavior* b = (Neo::LuaBehavior*) engine->getBehaviorManager()->getBehaviorByName("LuaScript")->getNewBehavior(object);
	NeoVariable var = b->getVariable(0);
	LT_ASSERT_EQ(var.getType(), M_VARIABLE_STRING);

	((MString*) var.getPointer())->set("does-not-exist.lua");
	b->update();

LT_END_AUTO_TEST(LuaBehaviorNonExistantScript_test);

#define CANVAS_TEST_SIZE 4
LT_BEGIN_AUTO_TEST(NeoTestSdk, GuiSystem_test);

    GuiSystem* gui = GuiSystem::getInstance();

    Canvas* canvases[CANVAS_TEST_SIZE];
    for(int i = 0; i < CANVAS_TEST_SIZE; i++)
    {
        Canvas* testc = new Canvas;

        canvases[i] = testc;
        testc->setLayer(i);

        gui->addCanvas(testc);
    }

    for(int i = 1; i < CANVAS_TEST_SIZE; i++)
    {
		LT_CHECK_EQ(gui->getCanvas(i)->getLayer(), i-1);
    }

LT_END_AUTO_TEST(GuiSystem_test);

LT_BEGIN_AUTO_TEST(NeoTestSdk, Messaging_test);

	Messenger* messenger = Messenger::getInstance();

	messenger->addInbox("TestThread", 0);
	LT_CHECK_EQ(messenger->getMessagesCount("TestThread"), 0);

	messenger->sendMessage("TestMessage", NULL, 0, "TestThread", "TestThread");
	LT_ASSERT_EQ(messenger->getMessagesCount("TestThread"), 1);

	Message msg = messenger->getNextMessage("TestThread");
	LT_CHECK_EQ(msg.message, "TestMessage");

LT_END_AUTO_TEST(Messaging_test);

LT_BEGIN_AUTO_TEST(NeoTestSdk, Object3dHandle_test);

	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
	Object3d* obj = scene->addNewGroup();

	LT_ASSERT(obj != NULL);

	unsigned long handle = obj->getId();
	LT_ASSERT_EQ(scene->getObjectByHandle(handle), obj);

LT_END_AUTO_TEST(Object3dHandle_test);

bool testFunctionSuccess = false;
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

	server.startServer(10, 60000);
	client.startClient("127.0.0.1", 60000);

	window->sleep(200);

	std::vector<NeoVariable> variables;
    variables.push_back(NeoVariable("Test", new MString("Hello World!"), M_VARIABLE_STRING));

	Messenger::getInstance()->sendMessage("testFunction", ID_RPC_MESSAGE, &variables, "ClientThread", "MainThread");

	window->sleep(1000);

    delete (MString*) variables[0].getPointer();
    variables.clear();

    LT_CHECK(testFunctionSuccess);

	game->end();

LT_END_AUTO_TEST(Networking_test);

LT_BEGIN_AUTO_TEST(NeoTestSdk, BitStreamArgumentString_test);

    RakNet::BitStream bs;
    writeString(&bs, "Hello String!");

    NeoVariable var = readNextArgument(&bs);

    LT_CHECK_EQ(var.getType(), M_VARIABLE_STRING);
    LT_ASSERT(var.getPointer() != NULL);

    MString* str = (MString*) var.getPointer();

    LT_CHECK_EQ(strcmp(str->getSafeString(), "Hello String!"), 0);

    delete str;

LT_END_AUTO_TEST(BitStreamArgumentString_test);

LT_BEGIN_AUTO_TEST(NeoTestSdk, BitStreamArgumentVector3_test);

    RakNet::BitStream bs;
    writeVector3(&bs, MVector3(1,2,3));

    NeoVariable var = readNextArgument(&bs);

    LT_CHECK_EQ(var.getType(), M_VARIABLE_VEC3);
    LT_ASSERT(var.getPointer() != NULL);

    MVector3* vec = (MVector3*) var.getPointer();

    LT_CHECK(*vec == MVector3(1,2,3));

    delete vec;

LT_END_AUTO_TEST(BitStreamArgumentVector3_test);

LT_BEGIN_TEST_ENV();
	AUTORUN_TESTS();
LT_END_TEST_ENV();
