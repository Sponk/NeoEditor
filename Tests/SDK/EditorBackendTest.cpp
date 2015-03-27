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


TEST_F(EditorBackendTest, Undo_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	Maratis* maratis = new Maratis();

	maratis->setRenderingContext(new DummyContext());
	maratis->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	// Create new object
	Object3d* obj = scene->addNewLight();
	obj->setName("Light");

	// Autosave
	maratis->autoSave();

	// Do some change
	obj->setName("LightNewName");

	// Check
	EXPECT_EQ(0, strcmp("LightNewName", obj->getName()));
	maratis->undo();

	// Now the old name should be re-fetched
	EXPECT_EQ(0, strcmp("Light", obj->getName()));
	
	delete maratis;
	resetEngine();
}

TEST_F(EditorBackendTest, MultiUndo_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	Maratis* maratis = new Maratis();

	maratis->setRenderingContext(new DummyContext());
	maratis->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	// Create new object
	Object3d* obj = scene->addNewLight();
	obj->setName("Light");

	// Autosave
	maratis->autoSave();

	// Do some change
	obj->setName("LightNewName");

	maratis->autoSave();

	// Do some change
	obj->setName("Name");
	
	// Check
	EXPECT_EQ(0, strcmp("Name", obj->getName()));
	maratis->undo();

	EXPECT_EQ(0, strcmp("LightNewName", obj->getName()));
	maratis->undo();
	
	// Now the old name should be re-fetched
	EXPECT_EQ(0, strcmp("Light", obj->getName()));
	
	delete maratis;
	resetEngine();
}

TEST_F(EditorBackendTest, DuplicateObjects_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	Maratis* maratis = new Maratis();

	maratis->setRenderingContext(new DummyContext());
	maratis->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	// Test duplicateObject
	Object3d* obj = scene->addNewCamera();
	obj->setName("Camera");

	obj = maratis->duplicateObject(obj);

	EXPECT_EQ(0, strcmp("Camera1", obj->getName()));

	// Test duplicateSelection
	maratis->addSelectedObject(obj);
	maratis->duplicateSelectedObjects();

	EXPECT_EQ(3, scene->getObjectsNumber());
	
	delete maratis;
	resetEngine();
}
