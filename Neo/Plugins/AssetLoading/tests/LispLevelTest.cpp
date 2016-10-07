#include <gtest/gtest.h>
#include <NeoEngine.h>
#include "LispLevel.h"

using namespace Neo;

class TestBehavior : public Behavior
{
	String m_string;
public:
	TestBehavior() : Behavior(NULL) {}

	virtual const char* getName(void) override
	{
		return "TestBehavior";
	}

	virtual unsigned int getVariablesNumber(void) override
	{
		return 1;
	}

	virtual NeoVariable getVariable(unsigned int id) override
	{
		switch(id)
		{
			case 0: return NeoVariable("String", &m_string, M_VARIABLE_STRING);
		}

		return NeoVariable();
	}

	virtual void destroy(void) override
	{

	}

	virtual Behavior* getCopy(Object3d* parentObject) override
	{
		return nullptr;
	}

	virtual void update(void) override
	{

	}

	static Behavior* getNew(Object3d*) { return new TestBehavior; }
};

TEST(LispLevelTest, SaveTest)
{
	Level level;

	auto scene = level.addNewScene();
	auto scene1 = level.addNewScene();

	scene->setScriptFilename("test.lua");
	scene->setName("SCENE1");
	scene1->setName("SCENE2");

	level.setCurrentScene(scene1);

	scene->addNewCamera()->setName("Camera0");
	scene->addNewLight()->setName("Light0");

	auto meshRef = MeshRef::getNew(new Mesh(), NULL);
	meshRef->getMesh()->allocMaterials(1);
	meshRef->getMesh()->allocTextures(1);

	auto mat = meshRef->getMesh()->addNewMaterial();
	auto tex = meshRef->getMesh()->addNewTexture(TextureRef::getNew(0, nullptr, 0));

	mat->allocTexturesPass(1);
	mat->addTexturePass(tex, TEX_COMBINE_ADD, 0);

	auto entity = scene->addNewEntity(meshRef);
	scene->addNewText(FontRef::getNew(NULL, NULL))->setName("Text0");
	scene->addNewSound(SoundRef::getNew(0, NULL))->setName("Sound0");

	TestBehavior behavior;
	entity->addBehavior(&behavior);

	char curdir[256];
	char filename[256];

	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");
	saveLispLevel(filename, "llvl", &level);
}

TEST(LispLevelTest, LoadTest)
{
	Level level;
	NeoEngine::getInstance()->getBehaviorManager()->addBehavior("TestBehavior", 0, TestBehavior::getNew);

	char curdir[256];
	char filename[256];

	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");
	ASSERT_TRUE(loadLispLevel(filename, &level));

	ASSERT_EQ(2, level.getScenesNumber());
	ASSERT_NE(nullptr, level.getSceneByIndex(0));
	ASSERT_NE(nullptr, level.getSceneByIndex(1));

	EXPECT_STREQ("SCENE1", level.getSceneByIndex(0)->getName());
	EXPECT_STREQ("SCENE2", level.getSceneByIndex(1)->getName());

	EXPECT_STREQ("test.lua", level.getSceneByIndex(0)->getScriptFilename());

	auto scene = level.getSceneByIndex(0);

	auto camera = scene->getCameraByName("Camera0");
	ASSERT_NE(nullptr, camera);
	EXPECT_EQ(0, camera->hasFog());
	EXPECT_EQ(0, camera->getFogDistance());
	EXPECT_EQ(Vector3(1,1,1), camera->getFogColor());
	EXPECT_EQ(Vector3(0.2f, 0.3f, 0.4f), camera->getClearColor());
	EXPECT_EQ(1, camera->getClippingNear());
	EXPECT_EQ(1000, camera->getClippingFar());
	EXPECT_EQ(60, camera->getFov());
	EXPECT_FALSE(camera->isOrtho());

	auto text = scene->getTextByName("Text0");
	ASSERT_NE(nullptr, text);
	EXPECT_EQ(0, text->getAlign());
	EXPECT_EQ(16, text->getSize());
}

TEST(LispLevelTest, LightTest)
{
	char curdir[256];
	char filename[256];
	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");

	// Create and save level
	{
		Level level;
		auto scene = level.addNewScene();

		scene->setName("scene-1");
		auto light = scene->addNewLight();
		light->setName("Light0");
		light->setColor(Vector3(1,0,0));
		light->setIntensity(0.25);
		light->setSpotAngle(90);
		light->setRadius(123);
		light->setSpotExponent(0.5);
		light->castShadow(true);
		light->setShadowBias(123);
		light->setShadowBlur(321);
		light->setShadowQuality(0xDEADBEEF);

		saveLispLevel(filename, "llvl", &level);
	}

	// Load level and check results
	{
		Level loadedlevel;
		ASSERT_TRUE(loadLispLevel(filename, &loadedlevel));

		auto scene = loadedlevel.getSceneByIndex(0);
		auto light = scene->getLightByName("Light0");
		ASSERT_NE(nullptr, light);
		EXPECT_EQ(Vector3(1, 0, 0), light->getColor());
		EXPECT_EQ(0.25, light->getIntensity());
		EXPECT_EQ(90, light->getSpotAngle());
		EXPECT_EQ(123, light->getRadius());
		EXPECT_EQ(0.5, light->getSpotExponent());
		EXPECT_TRUE(light->isCastingShadow());
		EXPECT_EQ(123, light->getShadowBias());
		EXPECT_EQ(321, light->getShadowBlur());
		EXPECT_EQ(0xDEADBEEF, light->getShadowQuality());
	}
}

TEST(LispLevelTest, SoundTest)
{
	char curdir[256];
	char filename[256];
	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");

	// Create and save level
	{
		Level level;
		auto scene = level.addNewScene();

		scene->setName("scene-1");
		auto sound = scene->addNewSound(SoundRef::getNew(0, nullptr));
		sound->setName("Sound0");
		sound->setPitch(0.5);
		sound->setGain(0.5);
		sound->setRadius(123);
		sound->setRolloff(2);
		sound->setLooping(true);
		sound->setRelative(true);

		saveLispLevel(filename, "llvl", &level);
	}

	// Load level and check results
	{
		Level loadedlevel;
		ASSERT_TRUE(loadLispLevel(filename, &loadedlevel));

		auto scene = loadedlevel.getSceneByIndex(0);
		auto sound = scene->getSoundByName("Sound0");
		ASSERT_NE(nullptr, sound);

		ASSERT_NE(nullptr, sound);
		EXPECT_EQ(0.5, sound->getPitch());
		EXPECT_EQ(0.5, sound->getGain());
		EXPECT_EQ(123, sound->getRadius());
		EXPECT_EQ(2, sound->getRolloff());
		EXPECT_TRUE(sound->isLooping());
		EXPECT_TRUE(sound->isRelative());
	}
}

TEST(LispLevelTest, TextTest)
{
	char curdir[256];
	char filename[256];
	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");

	// Create and save level
	{
		Level level;
		auto scene = level.addNewScene();

		scene->setName("scene-1");
		auto text = scene->addNewText(FontRef::getNew(0, nullptr));
		text->setName("Text0");
		text->setAlign(static_cast<TEXT_ALIGN_MODES>(1));
		text->setSize(32);

		saveLispLevel(filename, "llvl", &level);
	}

	// Load level and check results
	{
		Level loadedlevel;
		ASSERT_TRUE(loadLispLevel(filename, &loadedlevel));

		auto scene = loadedlevel.getSceneByIndex(0);
		auto text = scene->getTextByName("Text0");

		ASSERT_NE(nullptr, text);
		EXPECT_EQ(1, text->getAlign());
		EXPECT_EQ(32, text->getSize());
	}
}

TEST(LispLevelTest, EntityTest)
{
	char curdir[256];
	char filename[256];
	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");

	// Create and save level
	{
		Level level;
		auto scene = level.addNewScene();

		scene->setName("scene-1");
		auto meshRef = MeshRef::getNew(new Mesh(), NULL);
		meshRef->getMesh()->allocMaterials(1);
		meshRef->getMesh()->allocTextures(1);

		auto mat = meshRef->getMesh()->addNewMaterial();
		auto tex = meshRef->getMesh()->addNewTexture(TextureRef::getNew(0, nullptr, 0));

		mat->allocTexturesPass(1);
		mat->addTexturePass(tex, TEX_COMBINE_ADD, 0);

		scene->addNewEntity(meshRef)->setName("Entity0");

		saveLispLevel(filename, "llvl", &level);
	}

	// Load level and check results
	{
		Level loadedlevel;
		ASSERT_TRUE(loadLispLevel(filename, &loadedlevel));

		auto scene = loadedlevel.getSceneByIndex(0);
		auto entity = scene->getEntityByName("Entity0");

		ASSERT_NE(nullptr, entity);

		// TODO: More exact testing
		auto mesh = entity->getMesh();

		// All materials and textures are locally loaded into the entity
		// instead of the global mesh.
		EXPECT_EQ(0, mesh->getTexturesNumber());
		EXPECT_EQ(0, mesh->getMaterialsNumber());
	}
}

TEST(LispLevelTest, LinkTest)
{
	char curdir[256];
	char filename[256];
	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");

	// Create and save level
	{
		Level level;
		auto scene = level.addNewScene();

		scene->setName("scene-1");
		auto parent = scene->addNewGroup();
		auto child = scene->addNewGroup();

		parent->setName("parent");
		child->setName("child");
		child->linkTo(parent);

		saveLispLevel(filename, "llvl", &level);
	}

	// Load level and check results
	{
		Level loadedlevel;
		ASSERT_TRUE(loadLispLevel(filename, &loadedlevel));

		auto scene = loadedlevel.getSceneByIndex(0);
		auto parent = scene->getObjectByName("parent");
		auto child = scene->getObjectByName("child");

		ASSERT_NE(nullptr, parent);
		ASSERT_NE(nullptr, child);

		ASSERT_TRUE(child->hasParent());
		EXPECT_EQ(parent, child->getParent());
	}
}

TEST(LispLevelTest, PhysicsPropertiesTest)
{
	char curdir[256];
	char filename[256];
	getcwd(curdir, sizeof(curdir));
	getGlobalFilename(filename, curdir, "test.llvl");

	Mesh mesh;

	// Create and save level
	{
		Level level;
		auto scene = level.addNewScene();

		scene->setName("scene-1");
		auto entity = scene->addNewEntity(MeshRef::getNew(&mesh, ""));

		entity->setName("entity");
		auto phys = entity->createPhysicsProperties();
		phys->setCollisionShape(COLLISION_SHAPE_CONVEX_HULL);
		phys->setMass(15.0);
		phys->setFriction(15.0);
		phys->setGhost(true);
		phys->setRestitution(15.0);
		phys->setLinearDamping(15.0);
		phys->setAngularDamping(15.0);
		phys->setAngularFactor(15.0);
		phys->setLinearFactor(Vector3(1,2,3));

		auto con = phys->createConstraint();
		con->disableParentCollision = true;
		con->upperAngularLimit = Vector3(1,2,3);
		con->upperLinearLimit = Vector3(1,2,3);
		con->lowerAngularLimit = Vector3(1,2,3);
		con->lowerLinearLimit = Vector3(1,2,3);
		con->pivot = Vector3(1,2,3);
		con->parentName.set("parent");

		saveLispLevel(filename, "llvl", &level);
	}

	// Load level and check results
	{
		Level loadedlevel;
		ASSERT_TRUE(loadLispLevel(filename, &loadedlevel));

		auto scene = loadedlevel.getSceneByIndex(0);
		auto entity = scene->getEntityByName("entity");

		ASSERT_NE(nullptr, entity);
		ASSERT_NE(nullptr, entity->getPhysicsProperties());

		auto phys = entity->getPhysicsProperties();
		EXPECT_EQ(15, phys->getMass());
		EXPECT_EQ(15, phys->getRestitution());
		EXPECT_EQ(15, phys->getAngularDamping());
		EXPECT_EQ(15, phys->getLinearDamping());
		EXPECT_EQ(15, phys->getAngularFactor());
		EXPECT_EQ(15, phys->getFriction());

		EXPECT_EQ(COLLISION_SHAPE_CONVEX_HULL, phys->getCollisionShape());
		EXPECT_EQ(Vector3(1,2,3), *phys->getLinearFactor());

		auto con = phys->getConstraint();
		ASSERT_NE(nullptr, con);
		EXPECT_STREQ("parent", con->parentName.getSafeString());
		EXPECT_TRUE(con->disableParentCollision);
		EXPECT_EQ(Vector3(1,2,3), con->upperAngularLimit);
		EXPECT_EQ(Vector3(1,2,3), con->upperLinearLimit);
		EXPECT_EQ(Vector3(1,2,3), con->lowerAngularLimit);
		EXPECT_EQ(Vector3(1,2,3), con->lowerAngularLimit);
		EXPECT_EQ(Vector3(1,2,3), con->pivot);
	}
}