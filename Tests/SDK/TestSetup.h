
#ifndef _TEST_SETUP_H
#define _TEST_SETUP_H

#include <NeoEngine.h>
#include <LuaBehavior.h>
#include <WinContext.h>
#include <PackageManagerNPK.h>
#include <GuiSystem.h>
#include <SDLThread.h>
#include <Server.h>
#include <BulletContext.h>
#include <DummyContext.h>
#include <Window/Window.h>

#include <gtest/gtest.h>

class TestNeoSDK: public testing::Test
{
public:

	void SetUp()
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();

		Neo::NeoGame* game = new Neo::NeoGame();
		Neo::LuaScript* script = new Neo::LuaScript();
		Neo::Level* level = new Neo::Level();
		Neo::SystemContext* context = (Neo::SystemContext*) new Neo::MWinContext();
		Neo::PackageManager* pmanager = new Neo::PackageManagerNPK;
		Neo::BulletContext* physcontext = new Neo::BulletContext();
		Neo::RenderingContext* render = new Neo::DummyContext();
		
		pmanager->init();

		level->addNewScene();
		level->setCurrentSceneId(0);
		level->getCurrentScene()->setName("Scene-0");
		
		engine->setLevel(level);
		engine->setScriptContext(script);
		engine->setSystemContext(context);
		engine->setGame(game);
		engine->setPackageManager(pmanager);
		engine->setPhysicsContext(physcontext);
		engine->setRenderingContext(render);
		
		engine->getBehaviorManager()->addBehavior(Neo::LuaBehavior::getStaticName(), M_OBJECT3D, Neo::LuaBehavior::getNew);

		// Init default thread
		Neo::ThreadFactory* mgr = Neo::ThreadFactory::getInstance();
		mgr->setTemplateSemaphore(new Neo::SDLSemaphore());
		mgr->setTemplateThread(new Neo::SDLThread());

		game->begin();
	}

	void TearDown()
	{
		//MLOG_INFO("Clearing engine");
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();

		delete engine->getSystemContext();
		delete engine->getGame();
		delete engine->getLevel();
		delete engine->getScriptContext();
		delete engine->getPackageManager();

		Neo::ThreadFactory::getInstance()->clear();

		engine->setLevel(NULL);
		engine->setScriptContext(NULL);
		engine->setSystemContext(NULL);
		engine->setGame(NULL);
		engine->setPackageManager(NULL);
	}
};

#endif
