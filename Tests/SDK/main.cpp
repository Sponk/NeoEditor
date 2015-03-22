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
#include <DummyContext.h>

#include <gtest/gtest.h>
#include <NeoCore.h>
#include <Window/Window.h>

#ifdef main
#undef main
#endif

using namespace Neo;
using namespace Gui;

#include "TestSetup.h"

bool compare_float(float a, float b)
{
	return fabs(a - b) < 0.0001;
}

TEST_F(TestNeoSDK, Vector3_test)
{
	Vector3 testvec(1.0,1.0,1.0);
	testvec = testvec + testvec;
	EXPECT_EQ(testvec, Vector3(2.0,2.0,2.0));
}

TEST_F(TestNeoSDK, LuaBehavior_test)
{
	NeoEngine* engine = NeoEngine::getInstance();

	Object3d* object = engine->getLevel()->getCurrentScene()->addNewLight();
	Neo::LuaBehavior* b = (Neo::LuaBehavior*) engine->getBehaviorManager()->getBehaviorByName("LuaScript")->getNewBehavior(object);
	NeoVariable var = b->getVariable(0);

	EXPECT_EQ(var.getType(), M_VARIABLE_STRING);

	((String*) var.getPointer())->set("test.lua");
	int num = b->getVariablesNumber();

	ASSERT_EQ(num, 3);

	var = b->getVariable(0);
	ASSERT_EQ(var.getType(), M_VARIABLE_STRING);

	var = b->getVariable(2);
	ASSERT_EQ(M_VARIABLE_FLOAT, var.getType());

	ASSERT_FLOAT_EQ(123.123, *((float*)var.getPointer()));
}

TEST_F(TestNeoSDK, LuaBehaviorNonExistantScript_test)
{
	NeoEngine* engine = NeoEngine::getInstance();

	Object3d* object = engine->getLevel()->getCurrentScene()->addNewLight();
	Neo::LuaBehavior* b = (Neo::LuaBehavior*) engine->getBehaviorManager()->getBehaviorByName("LuaScript")->getNewBehavior(object);
	NeoVariable var = b->getVariable(0);
	ASSERT_EQ(M_VARIABLE_STRING, var.getType());

	((String*) var.getPointer())->set("does-not-exist.lua");
	b->update();
}

#define CANVAS_TEST_SIZE 4
TEST_F(TestNeoSDK, GuiSystem_test)
{

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
		EXPECT_EQ(i-1, gui->getCanvas(i)->getLayer());
    }
}

TEST_F(TestNeoSDK, Messenger_test)
{
	Messenger* messenger = Messenger::getInstance();

	messenger->addInbox("TestThread", 0);
	EXPECT_EQ(messenger->getMessagesCount("TestThread"), 0);

	messenger->sendMessage("TestMessage", 0, NULL, "TestThread", "TestThread");
	EXPECT_EQ(messenger->getMessagesCount("TestThread"), 1);

	Message msg = messenger->getNextMessage("TestThread");
	EXPECT_EQ(msg.message, "TestMessage");

}

TEST_F(TestNeoSDK, Object3dHandle_test)
{
	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
	Object3d* obj = scene->addNewGroup();

	ASSERT_NE((void*)NULL, obj);

	unsigned long handle = obj->getId();
	ASSERT_EQ(obj, scene->getObjectByHandle(handle));
}

class TestSubGame: public SubGame
{
public:
	void onBegin()
	{
		onBeginCalled = true;
	}

	void onEnd()
	{
		onEndCalled = true;
	}

	void update()
	{

	}

	void draw()
	{

	}

	bool onBeginCalled;
	bool onEndCalled;

	TestSubGame()
	{
		onBeginCalled = false;
		onEndCalled = false;
	}
};

TEST_F(TestNeoSDK, SubGame_test)
{
	NeoGame* game = NeoEngine::getInstance()->getGame();
	TestSubGame sg;

	game->registerSubGame(&sg);

	// Can't test update and draw here
	// since those need SDL to be initialized
	// with OpenGL
	game->onBegin();
	game->onEnd();

	EXPECT_TRUE(sg.onBeginCalled);
	EXPECT_TRUE(sg.onEndCalled);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
