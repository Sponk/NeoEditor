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
#include <Window/Window.h>
#include <SDLThread.h>

#include <gtest/gtest.h>
#include <NeoCore.h>

using namespace Neo;

class TestLevel: public testing::Test
{
public:

	void SetUp()
	{
		NeoEngine* engine = NeoEngine::getInstance();

		// Init default thread
		ThreadFactory* mgr = ThreadFactory::getInstance();
		mgr->setTemplateSemaphore(new SDLSemaphore());
		mgr->setTemplateThread(new SDLThread());
	}

	void TearDown()
	{
		ThreadFactory::getInstance()->clear();
	}
};

TEST_F(TestLevel, LevelRef_test)
{
	Level level;

	// The file does not exist.
	// It should return a valid pointer nonetheless!
	MeshRef* mref = level.loadMesh("Test.mesh");

	ASSERT_NE((void*) NULL, mref);
	EXPECT_EQ(1, mref->getScore());
}

TEST_F(TestLevel, LevelScene_test)
{
	Level* level = new Level();

	Scene* s = level->addNewScene();
	ASSERT_NE((void*) 0, s);

	s->setName("SomeScene");
	EXPECT_EQ(s, level->getSceneByName("SomeScene"));

	unsigned int idx;
	level->getSceneIndexByName("SomeScene", &idx);

	EXPECT_EQ(level->getCurrentSceneId(), idx);

	delete level;
}
