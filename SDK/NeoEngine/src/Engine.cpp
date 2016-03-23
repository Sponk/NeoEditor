//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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

using namespace Neo;

NeoEngine::NeoEngine(void):
m_isActive(true),
m_level(NULL),
m_game(NULL),
m_renderer(NULL),
m_soundContext(&m_soundContextDummy),
m_renderingContext(&m_renderingContextDummy),
m_physicsContext(&m_physicsContextDummy),
m_scriptContext(&m_scriptContextDummy),
m_inputContext(&m_inputContextDummy),
m_systemContext(&m_systemContextDummy),
m_packageManager(NULL),
m_requestedLevelToLoad(NULL)
{}

NeoEngine::~NeoEngine(void)
{
	SAFE_FREE(m_requestedLevelToLoad);
}

NeoEngine * NeoEngine::getInstance(void)
{
	static NeoEngine m_instance;
	return &m_instance;
}

void NeoEngine::setRenderingContext(RenderingContext * renderingContext)
{
	m_renderingContext = renderingContext;
}

void NeoEngine::setSoundContext(SoundContext * soundContext)
{
	m_soundContext = soundContext;
}

void NeoEngine::setPhysicsContext(PhysicsContext * physicsContext)
{
	m_physicsContext = physicsContext;
}

void NeoEngine::setScriptContext(ScriptContext * scriptContext)
{
	m_scriptContext = scriptContext;
}

void NeoEngine::setInputContext(InputContext * inputContext)
{
	m_inputContext = inputContext;
}

void NeoEngine::setSystemContext(SystemContext * systemContext)
{
	m_systemContext = systemContext;
}

void NeoEngine::setPackageManager(PackageManager * packageManager)
{
	m_packageManager = packageManager;
}

void NeoEngine::setLevel(Level * level)
{
	m_level = level;
}

void NeoEngine::updateRequests(void)
{
	loadLevelIfRequested();
	if(m_level)
		m_level->changeCurrentSceneIfRequested();
}

bool NeoEngine::loadLevel(const char * filename)
{
	if(!filename)
		return false;

	if(!m_level)
		return false;

	char globalFilename[256];
	getGlobalFilename(globalFilename, m_systemContext->getWorkingDirectory(), filename);

	// Check if there is a level under the given name
	if(!isFileExist(globalFilename))
	{
		MLOG_ERROR("Could not load level '" << filename << "', loading an empty level instead.");
		m_level->clear();
		m_level->clearScenes();
		m_level->addNewScene()->setName("Scene-0");

		return false;
	}

	m_renderer->stopThreads();

	if(m_game)
	{
		if(m_game->isRunning())
		{
			m_game->onEndScene();
			m_game->onEndLevel();
		}
	}

    if(m_levelLoader.loadData(globalFilename, m_level))
	{
		if(m_game)
		{
			if(m_game->isRunning())
			{
				m_game->onBeginLevel();
				m_game->onBeginScene();
			}
		}
		m_renderer->startThreads();
		return true;
	}

	MLOG_ERROR("Could not load level '" << filename << "', loading an empty level instead.");
	m_level->clear();
	m_level->clearScenes();
	m_level->addNewScene()->setName("Scene-0");

	if(m_game)
	{
		if(m_game->isRunning())
		{
			m_game->onBeginLevel();
			m_game->onBeginScene();
		}
	}

	m_renderer->startThreads();
	return false;
}

bool NeoEngine::saveLevel(const char *filename)
{
	return m_levelLoader.saveData(filename, "level", m_level);
}

void NeoEngine::requestLoadLevel(const char * filename)
{
	SAFE_FREE(m_requestedLevelToLoad);

	unsigned int len = strlen(filename);
	m_requestedLevelToLoad = (char*)malloc(len + 1);
	strcpy(m_requestedLevelToLoad, filename);
}

bool NeoEngine::doesLevelExist(const char * filename)
{
	if(! filename)
		return false;

	char globalFilename[256];
	getGlobalFilename(globalFilename, m_systemContext->getWorkingDirectory(), filename);
	return isFileExist(globalFilename);
}

void NeoEngine::setGame(NeoGame * game)
{
	m_game = game;
}

void NeoEngine::setRenderer(Renderer * renderer)
{
	m_renderer = renderer;
}

void NeoEngine::loadLevelIfRequested()
{
	if(m_requestedLevelToLoad == NULL)
		return;

	loadLevel(m_requestedLevelToLoad);
	SAFE_FREE(m_requestedLevelToLoad);
}
