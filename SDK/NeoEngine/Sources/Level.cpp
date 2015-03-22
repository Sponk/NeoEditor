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


#include <stdio.h>
#include "../Includes/NeoEngine.h"

using namespace Neo;

// level
Level::Level(void):
m_currentSceneId(0),
m_requestedSceneId(0xFFFFFFFF)
{}

Level::~Level(void)
{
	clear();
}

void Level::incrDataRefScore(DataRef * ref)
{
	if(! ref)
		return;

	if(ref->getScore() == 0)
		sendToUpdateQueue(ref);

	ref->incrScore();
}

void Level::decrDataRefScore(DataRef * ref)
{
	if(! ref)
		return;

	if(ref->getScore() == 1)
		sendToClearQueue(ref);

	ref->decrScore();
}

FontRef * Level::loadFont(const char * filename)
{
	unsigned int i;
	unsigned int size = m_fontManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		FontRef * ref = (FontRef *)m_fontManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(ref->getScore() == 0)
				ref->update();
			ref->incrScore();
			return ref;
		}
	}

	// add data
	FontRef * ref = FontRef::getNew(NULL, filename);
	m_fontManager.addRef(ref);
	if(ref->getScore() == 0)
		ref->update();
	ref->incrScore();
	return ref;
}

MeshRef * Level::loadMesh(const char * filename, const bool preload)
{
	unsigned int i;
	unsigned int size = m_meshManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		MeshRef * ref = (MeshRef *)m_meshManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(preload)
			{
				if(ref->getScore() == 0)
					ref->update();
				ref->incrScore();
			}

			return ref;
		}
	}

	// add data
	MeshRef * ref = MeshRef::getNew(NULL, filename);
	m_meshManager.addRef(ref);

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

ArmatureAnimRef * Level::loadArmatureAnim(const char * filename, const bool preload)
{
	unsigned int i;
	unsigned int size = m_armatureAnimManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		ArmatureAnimRef * ref = (ArmatureAnimRef *)m_armatureAnimManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(preload)
			{
				if(ref->getScore() == 0)
					ref->update();
				ref->incrScore();
			}

			return ref;
		}
	}

	// add data
	ArmatureAnimRef * ref = ArmatureAnimRef::getNew(NULL, filename);
	m_armatureAnimManager.addRef(ref);

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

TexturesAnimRef * Level::loadTexturesAnim(const char * filename, const bool preload)
{
	unsigned int i;
	unsigned int size = m_texturesAnimManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		TexturesAnimRef * ref = (TexturesAnimRef *)m_texturesAnimManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(preload)
			{
				if(ref->getScore() == 0)
					ref->update();
				ref->incrScore();
			}

			return ref;
		}
	}

	// add data
	TexturesAnimRef * ref = TexturesAnimRef::getNew(NULL, filename);
	m_texturesAnimManager.addRef(ref);

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

MaterialsAnimRef * Level::loadMaterialsAnim(const char * filename, const bool preload)
{
	unsigned int i;
	unsigned int size = m_materialsAnimManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		MaterialsAnimRef * ref = (MaterialsAnimRef *)m_materialsAnimManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(preload)
			{
				if(ref->getScore() == 0)
					ref->update();
				ref->incrScore();
			}

			return ref;
		}
	}

	// add data
	MaterialsAnimRef * ref = MaterialsAnimRef::getNew(NULL, filename);
	m_materialsAnimManager.addRef(ref);

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

SoundRef * Level::loadSound(const char * filename, const bool preload)
{
	unsigned int i;
	unsigned int size = m_soundManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		SoundRef * ref = (SoundRef *)m_soundManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(preload)
			{
				if(ref->getScore() == 0)
					ref->update();
				ref->incrScore();
			}

			return ref;
		}
	}

	// add data
	SoundRef * ref = SoundRef::getNew(0, filename);
	m_soundManager.addRef(ref);

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

TextureRef * Level::loadTexture(const char * filename, const bool mipmap, const bool preload)
{
	unsigned int i;
	unsigned int size = m_textureManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		TextureRef * ref = (TextureRef *)m_textureManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(preload)
			{
				if(ref->getScore() == 0)
					ref->update();
				ref->incrScore();
			}

			return ref;
		}
	}

	// add data
	TextureRef * ref = TextureRef::getNew(0, filename, mipmap);
	m_textureManager.addRef(ref);

	if(preload)
	{
		if(ref->getScore() == 0)
			ref->update();
		ref->incrScore();
	}

	return ref;
}

ShaderRef * Level::loadShader(const char * filename, M_SHADER_TYPES type)
{
	unsigned int i;
	unsigned int size = m_shaderManager.getRefsNumber();
	for(i=0; i<size; i++)
	{
		ShaderRef * ref = (ShaderRef *)m_shaderManager.getRef(i);
		if(strcmp(ref->getFilename(), filename) == 0)
		{
			if(ref->getScore() == 0)
				ref->update();
			ref->incrScore();
			return ref;
		}
	}

	ShaderRef * ref = ShaderRef::getNew(0, type, filename);
	m_shaderManager.addRef(ref);
	if(ref->getScore() == 0)
		ref->update();
	ref->incrScore();
	return ref;
}

FXRef * Level::createFX(ShaderRef * vertexShaderRef, ShaderRef * pixelShaderRef)
{
	RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

	unsigned int i;
	unsigned int size = m_FXManager.getFXRefsNumber();
	for(i=0; i<size; i++)
	{
		FXRef * fx_ref = (FXRef*) m_FXManager.getFXRef(i);
		if((fx_ref->getVertexShaderRef() == vertexShaderRef) && (fx_ref->getPixelShaderRef() == pixelShaderRef))
			return fx_ref;
	}

	unsigned int FXId;
	render->createFX(&FXId, vertexShaderRef->getShaderId(), pixelShaderRef->getShaderId());
	return m_FXManager.addFXRef(FXId, vertexShaderRef, pixelShaderRef);
}

void Level::sendToUpdateQueue(DataRef * ref)
{
	m_updateQueue.push_back(ref);
}

void Level::sendToClearQueue(DataRef * ref)
{
	m_clearQueue.push_back(ref);
}

void Level::updateQueueDatas(void)
{
	unsigned int i;
	for(i=0; i<m_updateQueue.size(); i++)
	{
		DataRef * ref = m_updateQueue[i];
		ref->update();
	}

	m_updateQueue.clear();
}

void Level::clearQueueDatas(void)
{
	unsigned int i;
	for(i=0; i<m_clearQueue.size(); i++)
	{
		DataRef * ref = m_clearQueue[i];
		ref->clear();
	}

	m_clearQueue.clear();
}

Scene * Level::addNewScene(void)
{
	Scene * scene = new Scene();
	m_scenes.push_back(scene);
	return scene;
}

void Level::clear(void)
{
	clearScenes();

	m_updateQueue.clear();
	m_clearQueue.clear();

	m_FXManager.clear();
	m_armatureAnimManager.clear();
	m_textureManager.clear();
	m_shaderManager.clear();
	m_soundManager.clear();
	m_meshManager.clear();
	m_fontManager.clear();
	m_armatureAnimManager.clear();
	m_texturesAnimManager.clear();
	m_materialsAnimManager.clear();
}

void Level::clearScenes(void)
{
	unsigned int i;
	unsigned int sSize = getScenesNumber();
	for(i=0; i<sSize; i++)
		SAFE_DELETE(m_scenes[i]);

	m_currentSceneId = 0;
	m_scenes.clear();
}

void Level::setCurrentSceneId(unsigned int id)
{
	if(id == m_currentSceneId)
	{
		updateQueueDatas();
		return;
	}

	// previous scene
	Scene * scene = getCurrentScene();
	if(scene->getDataMode() == M_DATA_DYNAMIC || scene->getDataMode() == M_DATA_STREAM)
	{
		// meshs
		unsigned int i;

		unsigned int eSize = scene->getEntitiesNumber();
		for(i=0; i<eSize; i++)
		{
			OEntity * entity = scene->getEntityByIndex(i);
			Mesh * mesh = entity->getMesh();
			if(mesh)
			{
				// textures
				unsigned int t;
				unsigned int tSize = mesh->getTexturesNumber();
				for(t=0; t<tSize; t++)
				{
					TextureRef * ref = mesh->getTexture(t)->getTextureRef();
					decrDataRefScore(ref);
				}

				// armature anim
				{
					ArmatureAnimRef * ref = mesh->getArmatureAnimRef();
					decrDataRefScore(ref);
				}

				// textures anim
				{
					TexturesAnimRef * ref = mesh->getTexturesAnimRef();
					decrDataRefScore(ref);
				}

				// materials anim
				{
					MaterialsAnimRef * ref = mesh->getMaterialsAnimRef();
					decrDataRefScore(ref);
				}
			}

			// mesh
			MeshRef * ref = entity->getMeshRef();
			decrDataRefScore(ref);

		}

		// sounds
		unsigned int sSize = scene->getSoundsNumber();
		for(i=0; i<sSize; i++)
		{
			OSound * sound = scene->getSoundByIndex(i);
			SoundRef * ref = sound->getSoundRef();
			decrDataRefScore(ref);
		}

		// clear queue data
		clearQueueDatas();
	}

	// change current id
	m_currentSceneId = id;

	// next scene
	scene = getCurrentScene();
	if(scene->getDataMode() == M_DATA_DYNAMIC || scene->getDataMode() == M_DATA_STREAM)
	{
		// meshs
		unsigned int i;

		unsigned int eSize = scene->getEntitiesNumber();
		for(i=0; i<eSize; i++)
		{
			OEntity * entity = scene->getEntityByIndex(i);
			Mesh * mesh = entity->getMesh();
			if(mesh)
			{
				// textures
				unsigned int t;
				unsigned int tSize = mesh->getTexturesNumber();
				for(t=0; t<tSize; t++)
				{
					TextureRef * ref = mesh->getTexture(t)->getTextureRef();
					incrDataRefScore(ref);
				}

				// armature anim
				{
					ArmatureAnimRef * ref = mesh->getArmatureAnimRef();
					incrDataRefScore(ref);
				}

				// textures anim
				{
					TexturesAnimRef * ref = mesh->getTexturesAnimRef();
					incrDataRefScore(ref);
				}

				// materials anim
				{
					MaterialsAnimRef * ref = mesh->getMaterialsAnimRef();
					incrDataRefScore(ref);
				}
			}

			// mesh
			MeshRef * ref = entity->getMeshRef();
			incrDataRefScore(ref);
		}

		// sounds
		unsigned int sSize = scene->getSoundsNumber();
		for(i=0; i<sSize; i++)
		{
			OSound * sound = scene->getSoundByIndex(i);
			SoundRef * ref = sound->getSoundRef();
			incrDataRefScore(ref);
		}
	}

	// updateQueueData
	updateQueueDatas();
}

void Level::changeCurrentScene(unsigned int id)
{
	m_requestedSceneId = id;
}

void Level::changeCurrentSceneIfRequested()
{
	if(m_requestedSceneId == 0xFFFFFFFF)
		return;

	if(m_requestedSceneId != m_currentSceneId)
	{
		NeoEngine * engine = NeoEngine::getInstance();

		if(m_requestedSceneId < getScenesNumber())
		{
			Scene * scene = getCurrentScene();

			// onEndScene
			NeoGame * game = engine->getGame();
			if(game)
				game->onEndScene();

			// change scene
			setCurrentSceneId(m_requestedSceneId);
			scene = getCurrentScene();

			// onBeginScene
			if(game)
				game->onBeginScene();
		}
	}

	m_requestedSceneId = 0xFFFFFFFF; // reset requested scene
}

void Level::deleteScene(unsigned int id)
{
	SAFE_DELETE(m_scenes[id]);
	m_scenes.erase(m_scenes.begin() + id);

	unsigned int sSize = getScenesNumber();
	if(sSize == 0)
	{
		m_currentSceneId = 0;
		return;
	}

	unsigned int maxId = (unsigned int)((int)getScenesNumber() - 1);
	if(m_currentSceneId > maxId)
		m_currentSceneId = maxId;
}

Scene * Level::getCurrentScene(void)
{
	unsigned int sSize = getScenesNumber();
	if(m_currentSceneId < sSize)
		return m_scenes[m_currentSceneId];

	return NULL;
}

Scene * Level::getSceneByName(const char * name)
{
	unsigned int i;
	unsigned int sSize = getScenesNumber();
	for(i=0; i<sSize; i++)
	{
		if(! strcmp(name, m_scenes[i]->getName()))
			return m_scenes[i];
	}

	return NULL;
}

bool Level::getSceneIndexByName(const char * name, unsigned int * index)
{
	unsigned int i;
	unsigned int sSize = getScenesNumber();
	for(i=0; i<sSize; i++)
	{
		if(! strcmp(name, m_scenes[i]->getName()))
		{
			(*index) = i;
			return true;
		}
	}

	return false;
}
