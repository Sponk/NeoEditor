
#ifndef _TEST_SETUP_H
#define _TEST_SETUP_H

class TestNeoSDK: public testing::Test
{
public:

	void SetUp()
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

	void TearDown()
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

#endif
