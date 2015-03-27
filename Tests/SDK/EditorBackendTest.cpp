#include <gtest/gtest.h>
#include "TestSetup.h"
#include <Maratis.h>

using namespace Neo;

class EditorBackendTest: public TestNeoSDK
{
public:
	// The backed class will do the tear down.
	// Just ignore it.
	void TearDown()
	{

	}

	void SetUp()
	{

	}
};

void resetEngine()
{
	NeoEngine* engine = NeoEngine::getInstance();
	// FIXME: No manual cleanup!
	engine->setLevel(NULL);
	engine->setScriptContext(NULL);
	engine->setSystemContext(NULL);
	engine->setGame(NULL);
	engine->setPackageManager(NULL);
}

TEST_F(EditorBackendTest, SaveLevel_test)
{
	
	NeoEngine* engine = NeoEngine::getInstance();
	Maratis* maratis = new Maratis();

	maratis->setRenderingContext(new DummyContext());
	maratis->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	Object3d* obj = scene->addNewLight();
	obj->setName("Light");
	
	obj = scene->addNewCamera();
	obj->setName("Camera");

	maratis->okSaveAs("test.level");

	// TODO: More serious checking!
	EXPECT_EQ(true, isFileExist("test.level"));

	delete maratis;
	resetEngine();
}

TEST_F(EditorBackendTest, LoadLevel_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	Maratis* maratis = new Maratis();

	maratis->setRenderingContext(new DummyContext());
	maratis->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	maratis->loadLevel("test.level");

	EXPECT_TRUE(scene->getObjectByName("Light") != NULL);
	EXPECT_TRUE(scene->getObjectByName("Camera") != NULL);

	delete maratis;
	resetEngine();
}
