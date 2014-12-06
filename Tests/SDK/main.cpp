#include <MEngine.h>
#include <MCore.h>
#include <LuaBehavior.h>
#include <MWindow.h>
#include <WinContext.h>
#include <PackageManagerNPK.h>
#include "liblittletest.hpp"

bool compare_float(float a, float b)
{
	return fabs(a - b) < 0.0001;
}

LT_BEGIN_SUITE(NeoTestSdk);

void set_up()
{
	//MLOG_INFO("Setting up engine");
	MEngine* engine = MEngine::getInstance();

	MGame* game = new MGame();
	Neo::LuaScript* script = new Neo::LuaScript();
	MLevel* level = new MLevel();
	MSystemContext* context = (MSystemContext*) new Neo::MWinContext();
	MPackageManager* pmanager = new Neo::MPackageManagerNPK;

	pmanager->init();

	level->addNewScene();
	level->setCurrentSceneId(0);

	engine->setLevel(level);
	engine->setScriptContext(script);
	engine->setSystemContext(context);
	engine->setGame(game);
	engine->setPackageManager(pmanager);

	engine->getBehaviorManager()->addBehavior(Neo::LuaBehavior::getStaticName(), M_OBJECT3D, Neo::LuaBehavior::getNew);

	game->begin();
}

void tear_down()
{
	//MLOG_INFO("Clearing engine");
	MEngine* engine = MEngine::getInstance();

	delete engine->getSystemContext();
	delete engine->getGame();
	delete engine->getLevel();
	delete engine->getScriptContext();
	delete engine->getPackageManager();

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

	MEngine* engine = MEngine::getInstance();

	MObject3d* object = engine->getLevel()->getCurrentScene()->addNewLight();
	Neo::LuaBehavior* b = (Neo::LuaBehavior*) engine->getBehaviorManager()->getBehaviorByName("LuaScript")->getNewBehavior(object);
	MVariable var = b->getVariable(0);

	LT_ASSERT(var.getType() == M_VARIABLE_STRING);

	((MString*) var.getPointer())->set("test.lua");
	int num = b->getVariablesNumber();

	LT_ASSERT(num == 3);

	var = b->getVariable(1);
	LT_ASSERT(var.getType() == M_VARIABLE_STRING);

	var = b->getVariable(2);
	LT_ASSERT(var.getType() == M_VARIABLE_FLOAT);

	LT_ASSERT(compare_float(*((float*)var.getPointer()), 123.123));

LT_END_AUTO_TEST(LuaBehavior_test);

LT_BEGIN_TEST_ENV();
	AUTORUN_TESTS();
LT_END_TEST_ENV();
