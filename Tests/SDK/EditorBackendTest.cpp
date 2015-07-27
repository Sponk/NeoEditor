#include <gtest/gtest.h>
#include "TestSetup.h"
#include <EditorBackend.h>

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
	engine->setRenderingContext(NULL);
	engine->setRenderer(NULL);
  
}

TEST_F(EditorBackendTest, SaveLevel_test)
{
	
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);

	backend->initRenderer();
	backend->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	Object3d* obj = scene->addNewLight();
	obj->setName("Light");
	
	obj = scene->addNewCamera();
	obj->setName("Camera");

	backend->okSaveAs("test.level");

	// TODO: More serious checking!
	EXPECT_EQ(true, isFileExist("test.level"));

	delete backend;
	resetEngine();
}

TEST_F(EditorBackendTest, LoadLevel_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);
	
	backend->initRenderer();
	backend->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	backend->loadLevel("test.level");

	EXPECT_TRUE(scene->getObjectByName("Light") != NULL);
	EXPECT_TRUE(scene->getObjectByName("Camera") != NULL);

	delete backend;
	resetEngine();
}


TEST_F(EditorBackendTest, Undo_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);
	
	backend->initRenderer();
	backend->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	// Create new object
	Object3d* obj = scene->addNewLight();
	obj->setName("Light");

	// Autosave
	backend->autoSave();

	// Do some change
	obj->setName("LightNewName");

	// Check
	EXPECT_EQ(0, strcmp("LightNewName", obj->getName()));
	backend->undo();

	// Now the old name should be re-fetched
	EXPECT_EQ(0, strcmp("Light", obj->getName()));
	
	delete backend;
	resetEngine();
}

TEST_F(EditorBackendTest, MultiUndo_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);
	
	backend->initRenderer();
	backend->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	// Create new object
	Object3d* obj = scene->addNewLight();
	obj->setName("Light");

	// Autosave
	backend->autoSave();

	// Do some change
	obj->setName("LightNewName");

	backend->autoSave();

	// Do some change
	obj->setName("Name");
	
	// Check
	EXPECT_EQ(0, strcmp("Name", obj->getName()));
	backend->undo();

	EXPECT_EQ(0, strcmp("LightNewName", obj->getName()));
	backend->undo();
	
	// Now the old name should be re-fetched
	EXPECT_EQ(0, strcmp("Light", obj->getName()));
	
	delete backend;
	resetEngine();
}

TEST_F(EditorBackendTest, DISABLED_DuplicateObjects_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);
	
	backend->initRenderer();
	backend->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	// Test duplicateObject
	Object3d* obj = scene->addNewCamera();
	obj->setName("Camera");

	obj = backend->duplicateObject(obj);

	EXPECT_EQ(0, strcmp("Camera1", obj->getName()));

	// Test duplicateSelection
	backend->addSelectedObject(obj);
	backend->duplicateSelectedObjects();

	EXPECT_EQ(3, scene->getObjectsNumber());
	
	delete backend;
	resetEngine();
}

TEST_F(EditorBackendTest, Redo_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);
	
	backend->initRenderer();
	backend->start();

	Scene* scene = engine->getLevel()->getCurrentScene();

	// Create new object
	Object3d* obj = scene->addNewLight();
	obj->setName("Light");

	// Autosave
	backend->autoSave();

	// Do some change
	obj->setName("LightNewName");

	// Check
	EXPECT_EQ(0, strcmp("LightNewName", obj->getName()));
	backend->undo();

	// Now the old name should be re-fetched
	EXPECT_EQ(0, strcmp("Light", obj->getName()));

	// redo
	backend->redo();
	
	// Check
	EXPECT_EQ(0, strcmp("LightNewName", obj->getName()));
	backend->undo();

	// Now the old name should be re-fetched
	EXPECT_EQ(0, strcmp("Light", obj->getName()));

	delete backend;
	resetEngine();
}

TEST_F(EditorBackendTest, DISABLED_ProjectSave_test)
{
	
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);
	
	backend->initRenderer();
	backend->start();

	engine->getGame()->enablePostEffects();
	PostProcessor* pp = engine->getGame()->getPostProcessor();
	pp->setShaderPath("shad.vert", "shad.frag");	
	
	Scene* scene = engine->getLevel()->getCurrentScene();

	Object3d* obj = scene->addNewLight();
	obj->setName("Light");
	
	obj = scene->addNewCamera();
	obj->setName("Camera");

	backend->okSaveAs("test.level");
	backend->okNewProject("test.mproj");

	// TODO: More serious checking!
	EXPECT_EQ(true, isFileExist("test.level"));
	EXPECT_EQ(true, isFileExist("test.mproj"));
	
	delete backend;
	resetEngine();
}

TEST_F(EditorBackendTest, DISABLED_ProjectLoad_test)
{
	
	NeoEngine* engine = NeoEngine::getInstance();
	EditorBackend* backend = new EditorBackend();
	RenderingContext* render = new DummyContext();

	backend->setRenderingContext(render);
	engine->setRenderingContext(render);
	
	backend->initRenderer();
	backend->start();

	// Load the project
	backend->okLoadProject("test.mproj");

	// Fetch the post processor
	PostProcessor* pp = engine->getGame()->getPostProcessor();
	EXPECT_EQ(0, strcmp("shad.vert", pp->getVertexShader()));
	
	delete backend;
	resetEngine();
}
