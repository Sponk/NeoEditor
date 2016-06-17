//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
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

static unsigned int s_renderBufferId = 0;

NeoGame::NeoGame(void):
m_isRunning(false),
m_frameDelta(0),
m_lastFrame(0),
m_drawMainScene(true)
{}

NeoGame::~NeoGame(void)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

	// delete frame buffer
	if(s_renderBufferId != 0)
		render->deleteFrameBuffer(&s_renderBufferId);
}

void NeoGame::update(void)
{ 
	NeoEngine* engine = NeoEngine::getInstance();

	engine->getInputContext()->handleInput();

	long int curtime = engine->getSystemContext()->getSystemTick();
	m_frameDelta = (float) (curtime - m_lastFrame) * 0.001; 
	m_lastFrame = curtime;

	// get level
	Level* level = NeoEngine::getInstance()->getLevel();
	if(!level)
		return;

	// get current scene
	Scene* scene = level->getCurrentScene();
	if(!scene)
		return;

#ifndef DISABLE_3D
	engine->getRenderer()->stopThreads();
	// update behaviors
	scene->updateObjectsBehaviors();

	// update scene
	scene->update();

	// update physics
	scene->updatePhysics();

	// update objects matrices
	scene->updateObjectsMatrices();

	// update scene layer
	OCamera * camera = scene->getCurrentCamera();
	if(camera)
	{
		unsigned int sceneLayerId = camera->getSceneLayer();
		if(sceneLayerId > 0 && sceneLayerId <= level->getScenesNumber())
		{
			Scene * sceneLayer = level->getSceneByIndex(sceneLayerId-1);

			sceneLayer->updateObjectsBehaviors();
			sceneLayer->update();
			sceneLayer->updateObjectsMatrices();
		}
	}
#endif

	// Update subgames
	for(auto sg : m_subGames)
	{
		sg->update();
	}

#ifndef DISABLE_3D
	engine->getRenderer()->startThreads();
#endif

	// flush input
	engine->getInputContext()->flush();
}

void NeoGame::draw(void)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

	// get level
	Level* level = NeoEngine::getInstance()->getLevel();
	if(!level)
		return;

	// get current scene
	Scene* scene = level->getCurrentScene();
	if(!scene)
		return;

	if(!m_drawMainScene)
	{
		render->clear(BUFFER_COLOR | BUFFER_DEPTH);
		goto skip_draw_mainscene; /// EVIL!
	}

#ifndef DISABLE_3D
	// render to texture
	{
		unsigned int currentFrameBuffer = 0;
		render->getCurrentFrameBuffer(&currentFrameBuffer);

		int viewport[4];
		bool recoverViewport = false;

		unsigned int c, cSize = scene->getCamerasNumber();
		for(c=0; c<cSize; c++)
		{
			OCamera* camera = scene->getCameraByIndex(c);
			
			if(camera->isActive() && camera->getRenderColorTexture())
			{
				if(!recoverViewport)
				{
					render->getViewport(viewport);
					recoverViewport = true;
				}

				TextureRef * colorTexture = camera->getRenderColorTexture();
				TextureRef * depthTexture = camera->getRenderDepthTexture();

				unsigned int width = colorTexture->getWidth();
				unsigned int height = colorTexture->getHeight();

				// render buffer
				if(s_renderBufferId == 0)
					render->createFrameBuffer(&s_renderBufferId);

				render->bindFrameBuffer(s_renderBufferId);

				render->enableDepthTest();

				render->attachFrameBufferTexture(ATTACH_COLOR0, colorTexture->getTextureId());
				if(depthTexture)
					render->attachFrameBufferTexture(ATTACH_DEPTH, depthTexture->getTextureId());

				render->setViewport(0, 0, width, height);
				render->setClearColor(camera->getClearColor());
				render->clear(BUFFER_COLOR | BUFFER_DEPTH);

				// draw the scene
				camera->enable();
				scene->draw(camera);

				// finish render to texture
				render->bindFrameBuffer(currentFrameBuffer);
			}
		}
#else
		render->clear(BUFFER_COLOR | BUFFER_DEPTH);
#endif

#ifndef DISABLE_3D

		// recover viewport
		if(recoverViewport)
			render->setViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	}

	// depth test
	render->enableDepthTest();

	// render scene
	if(scene->getCamerasNumber() == 0)
	{
		// draw scene with default camera
		OCamera camera;

		render->setClearColor(camera.getClearColor());
		render->clear(BUFFER_COLOR | BUFFER_DEPTH);
		render->disableScissorTest();

		camera.enable();
		camera.updateListener();
		scene->draw(&camera);
		scene->drawObjectsBehaviors();
	}
	else
	{
		OCamera * camera = scene->getCurrentCamera();
		render->disableScissorTest();

		// draw current scene
        if(!camera->getRenderColorTexture())
		{
			render->setClearColor(camera->getClearColor());
			render->clear(BUFFER_COLOR | BUFFER_DEPTH);

			camera->enable();
			camera->updateListener();
			scene->draw(camera);
			scene->drawObjectsBehaviors();
		}


		// draw scene layer
		unsigned int sceneLayerId = camera->getSceneLayer();
		if(sceneLayerId > 0 && sceneLayerId <= level->getScenesNumber())
		{
			Scene * sceneLayer = level->getSceneByIndex(sceneLayerId-1);
			OCamera * layerCamera = sceneLayer->getCurrentCamera();
			if(layerCamera)
			{
				layerCamera->enable();
				camera = layerCamera;
			}

			// draw on top
			render->clear(BUFFER_DEPTH);
			sceneLayer->draw(camera);
			sceneLayer->drawObjectsBehaviors();
		}
	}
#endif

skip_draw_mainscene:
	for(auto sg : m_subGames)
		sg->draw();
}

void NeoGame::onBeginScene(void)
{
	// get level
	NeoEngine* engine = NeoEngine::getInstance(); 
	Level * level = engine->getLevel();
	if(! level)
		return;

	// get current scene
	Scene * scene = level->getCurrentScene();
	if(! scene)
		return;

	// begin scene
	scene->begin();
}

void NeoGame::onEndScene(void)
{
	NeoEngine* engine = NeoEngine::getInstance();

#ifndef DISABLE_3D
	engine->getPhysicsContext()->clear();
#endif

	// get level
	Level * level = NeoEngine::getInstance()->getLevel();
	if(! level)
		return;

	// get current scene
	Scene * scene = level->getCurrentScene();
	if(! scene)
		return;

	// end scene
	scene->end();
}

void NeoGame::onBegin()
{
	NeoEngine::getInstance()->getRenderer()->initialize();
	for(int i = 0; i < m_subGames.size(); i++)
	{
		m_subGames[i]->onBegin();
	}
}

void NeoGame::onEnd(void)
{
	for(int i = 0; i < m_subGames.size(); i++)
	{
		m_subGames[i]->onEnd();
	}

	MLOG_INFO(std::endl << m_profiler.generateReport().getSafeString());
}
