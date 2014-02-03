/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// Maratis.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================


#include "MaratisPlayer.h"
#include <MWindow.h>

// MaratisCore
#include <MContexts/MGLContext.h>
#include <MContexts/MALContext.h>
#include <MContexts/MBulletContext.h>
#include <MContexts/MWinContext.h>
#include <MLoaders/MDevILLoader.h>
#include <MLoaders/MSndFileLoader.h>
#include <MLoaders/MFreetypeLoader.h>
#include <MLoaders/MBinFontLoader.h>
#include <MLoaders/MBinMeshLoader.h>

#include <MFileManager/MLevelLoad.h>
#include <MBehaviors/MBLookAt.h>
#include <MBehaviors/MBFollow.h>
#include <MScript/MScript.h>
#include <MInput/MInput.h>
#include <MFileManager/MMeshLoad.h>
#include <MFileManager/MLevelLoad.h>
#include <MFileManager/MPackageManagerNPK.h>
#include <MRenderers/MStandardRenderer.h>
#include <MRenderers/MFixedRenderer.h>


MaratisPlayer::MaratisPlayer(void):
m_gamePlugin(NULL),
m_renderer(NULL)
{
	// MEngine
	{
		m_soundContext = new MALContext();
		m_render = new MGLContext();
		m_physics = new MBulletContext();
		m_script = new MScript();
		m_input = new MInput();
		m_system = new MWinContext();
		m_level = new MLevel();
		m_game = new MGame();
		m_packageManager = new MPackageManagerNPK();
		
		m_physics->setSimulationQuality(2);
	}

	// start
	start();
}

MaratisPlayer::~MaratisPlayer(void)
{
	clear();

	SAFE_DELETE(m_game);
	SAFE_DELETE(m_level);

	SAFE_DELETE(m_render);
	SAFE_DELETE(m_soundContext);
	SAFE_DELETE(m_physics);
	SAFE_DELETE(m_script);
	SAFE_DELETE(m_input);
	SAFE_DELETE(m_system);
	SAFE_DELETE(m_packageManager);
}

void MaratisPlayer::changeRenderer(const char * name)
{
	MEngine * engine = MEngine::getInstance();
	MRendererManager * rendererManager = engine->getRendererManager();
	
	MRendererCreator * renderer = rendererManager->getRendererByName(name);
	if(renderer)
	{
		if(m_renderer)
			m_renderer->destroy();
		m_renderer = renderer->getNewRenderer();
		engine->setRenderer(m_renderer);
	}
}

void MaratisPlayer::start(void)
{
	// MEngine
	{
		MEngine * engine = MEngine::getInstance();

		// package manager
		engine->setPackageManager(m_packageManager);
		m_packageManager->init();
		
		// contexts
		engine->setSoundContext(m_soundContext); // sound context
		engine->setRenderingContext(m_render); // rendering context
		engine->setPhysicsContext(m_physics); // physics context
		engine->setScriptContext(m_script); // script context
		engine->setInputContext(m_input); // input context
		engine->setSystemContext(m_system); // system context

		// loaders
		engine->getImageLoader()->addLoader(M_loadImage); // image loader
		engine->getSoundLoader()->addLoader(M_loadSound); // sound loader
		engine->getLevelLoader()->addLoader(xmlLevelLoad); // level loader
		engine->getFontLoader()->addLoader(M_loadFont); // font loader
		engine->getFontLoader()->addLoader(M_loadBinFont); // bin font loader

		// add behaviors
		engine->getBehaviorManager()->addBehavior(MBLookAt::getStaticName(), M_OBJECT3D_CAMERA, MBLookAt::getNew);
		engine->getBehaviorManager()->addBehavior(MBFollow::getStaticName(), M_OBJECT3D, MBFollow::getNew);

		// add renderers
		engine->getRendererManager()->addRenderer(MStandardRenderer::getStaticName(), MStandardRenderer::getNew);
		engine->getRendererManager()->addRenderer(MFixedRenderer::getStaticName(), MFixedRenderer::getNew);
		
		// mesh loader
		engine->getMeshLoader()->addLoader(xmlMeshLoad);
		engine->getMeshLoader()->addLoader(M_loadBinMesh);
		engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
		engine->getArmatureAnimLoader()->addLoader(M_loadBinArmatureAnim);
		engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
		engine->getTexturesAnimLoader()->addLoader(M_loadBinTexturesAnim);
		engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);
        engine->getMaterialsAnimLoader()->addLoader(M_loadBinMaterialsAnim);

		// level
		engine->setLevel(m_level);

		// game
		engine->setGame(m_game);
		
		// set default renderer (standard)
		if(m_renderer == NULL)
			m_renderer = new MStandardRenderer();
		engine->setRenderer(m_renderer);
	}
}

void MaratisPlayer::clear(void)
{
	MEngine * engine = MEngine::getInstance();
	
	// level
	m_level->clear();
	if(m_renderer){
		m_renderer->destroy();
		m_renderer = NULL;
	}
	
	// plugins
	SAFE_DELETE(m_gamePlugin);

	unsigned int i;
	unsigned int pSize = m_plugins.size();
	for(i=0; i<pSize; i++)
		SAFE_DELETE(m_plugins[i]);

	m_plugins.clear();

	// MEngine
	{
		// loaders
		engine->getImageLoader()->clear();
		engine->getSoundLoader()->clear();
		engine->getLevelLoader()->clear();
		engine->getFontLoader()->clear();
		engine->getFontLoader()->clear();

		// behaviors
		engine->getBehaviorManager()->clear();
		
		// renderer
		engine->getRendererManager()->clear();

		// mesh loader
		engine->getMeshLoader()->clear();
		engine->getArmatureAnimLoader()->clear();
		engine->getTexturesAnimLoader()->clear();
		engine->getMaterialsAnimLoader()->clear();
	}
}

void MaratisPlayer::restart(void)
{
	clear();
	start();
}

void MaratisPlayer::loadGamePlugin(void)
{
	char gameFile[256];
	MWindow * window = MWindow::getInstance();

	#ifdef WIN32
		getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.dll");
	#elif __APPLE__
		getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.dylib");
	#elif linux
		getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.so");
	#endif

	SAFE_DELETE(m_gamePlugin);
	m_gamePlugin = new MPlugin();
	m_gamePlugin->load(gameFile);
}

bool MaratisPlayer::loadProject(const char * filename)
{
	if(! filename)
		return false;
	
	// load project file
	MProject proj;
	if(proj.loadXML(filename))
	{
		loadProject(&proj, filename);
		return true;
	}
	
	return false;
}

void MaratisPlayer::loadProject(MProject* proj, const char * filename)
{
	MWindow * window = MWindow::getInstance();
	MEngine * engine = MEngine::getInstance();
	
	
	// working directory
	char workingDir[256];
	getRepertory(workingDir, filename);
	window->setWorkingDirectory(workingDir);
	
	// restart
	restart();
	loadGamePlugin();
	
	// renderer
	changeRenderer(proj->renderer.c_str());
	
	// if we have a package manager, try to load the package
	if(MPackageManager* pPackMan = MEngine::getInstance()->getPackageManager())
	{
		char projName[256];
		getLocalFilename(projName, workingDir, filename);
		if(char* ext = strstr(projName, ".mproj"))
		{
			sprintf(ext, ".npk");
			char packageFile[256];
			getGlobalFilename(packageFile, workingDir, projName);
			pPackMan->loadPackage(packageFile);
		}
	}
	
	// load start level
	engine->loadLevel(proj->startLevel.c_str());
}

void MaratisPlayer::logicLoop(void)
{
	MEngine * engine = MEngine::getInstance();

	// game
	MGame * game = engine->getGame();
	if(game)
	{
		if(game->isRunning()){
			game->update();
		}
	}
}

void MaratisPlayer::graphicLoop(void)
{
	MWindow * window = MWindow::getInstance();
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	// game
	MGame * game = engine->getGame();
	if(game)
	{
		if(game->isRunning())
		{
			render->disableScissorTest();
			render->setViewport(0, 0, window->getWidth(), window->getHeight());
			game->draw();
		}
		else
		{
			render->clear(M_BUFFER_COLOR);
		}
	}
	else
	{
		render->clear(M_BUFFER_COLOR);
	}
}
