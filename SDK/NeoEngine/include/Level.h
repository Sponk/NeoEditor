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


#ifndef __LEVEL_H
#define __LEVEL_H


// level
namespace Neo
{
/**
 * @brief The Level class contains all scenes and assets that are loaded.
 *
 * When loading a level, all assets like textures, shaders, fonts, sounds and meshes are only loaded
 * once into memory for a maximum of efficiency. As soon as an asset is not used anymore it
 * will be unloaded and the RAM it used will be free'd what allows to clean up
 * during runtime.
 */
class NEO_ENGINE_EXPORT Level
{
private:

	// FX manager
	FXManager m_FXManager;

	// data managers
	DataManager m_fontManager;
	DataManager m_textureManager;
	DataManager m_shaderManager;
	DataManager m_soundManager;
	DataManager m_meshManager;
	DataManager m_armatureAnimManager;
	DataManager m_texturesAnimManager;
	DataManager m_materialsAnimManager;

	// data queues
	vector <DataRef *> m_updateQueue;
	vector <DataRef *> m_clearQueue;

	// scenes
	unsigned int		m_currentSceneId;
	unsigned int		m_requestedSceneId;
	vector <Scene *>	m_scenes;

public:

	// constructors / destructors
	 Level(void);
	 ~Level(void);

private:

	friend class NeoEngine;
	
	void incrDataRefScore(DataRef * ref);
	void decrDataRefScore(DataRef * ref);
	void changeCurrentSceneIfRequested();

public:

	// clear
	void clear(void);
	void clearScenes(void);

	// FX manager
	inline FXManager * getFXManager(void){ return &m_FXManager; }
	FXRef * createFX(ShaderRef * vertexShaderRef, ShaderRef * pixelShaderRef);

	// data managers
	inline DataManager * getFontManager(void){ return &m_fontManager; }
	inline DataManager * getTextureManager(void){ return &m_textureManager; }
	inline DataManager * getShaderManager(void){ return &m_shaderManager; }
	inline DataManager * getSoundManager(void){ return &m_soundManager; }
	inline DataManager * getMeshManager(void){ return &m_meshManager; }
	inline DataManager * getArmatureAnimManager(void){ return &m_armatureAnimManager; }
	inline DataManager * getTexturesAnimManager(void){ return &m_texturesAnimManager; }
	inline DataManager * getMaterialsAnimManager(void){ return &m_materialsAnimManager; }

	/**
	 * @brief Loads a font from a file and returns the new FontRef object.
	 *
	 * This method will never return NULL, even if the specified file does not exist
	 * or can not be read!
	 *
	 * This means you have to ensure the file exists before calling this method!
	 *
	 * @param filename The path to the file.
	 * @param fontsize The font size to load.
	 * @return The new FontRef object.
	 */
	FontRef * loadFont(const char * filename, unsigned int fontsize = 128);

	/**
	 * @brief Loads a mesh from a file and returns the new MeshRef object.
	 *
	 * This method will never return NULL, even if the specified file does not exist
	 * or can not be read!
	 *
	 * This means you have to ensure the file exists before calling this method!
	 *
	 * @param filename The file to load from.
	 * @param preload Allows to preload the file.
	 * @return The new MeshRef
	 */
	MeshRef * loadMesh(const char * filename, const bool preload = true);
	ArmatureAnimRef * loadArmatureAnim(const char * filename, const bool preload = true);
	TexturesAnimRef * loadTexturesAnim(const char * filename, const bool preload = true);
	MaterialsAnimRef * loadMaterialsAnim(const char * filename, const bool preload = true);

	/**
	 * @brief Loads a texture from a file and returns the new TextureRef object.
	 *
	 * This method will never return NULL, even if the specified file does not exist
	 * or can not be read!
	 *
	 * This means you have to ensure the file exists before calling this method!
	 *
	 * @param filename The file to load from.
	 * @param mipmap Indicates if a mip map should be generated for this texture.
	 * @param preload Allows to preload the file.
	 * @return The new TextureRef
	 */
	TextureRef * loadTexture(const char * filename, const bool mipmap = true, const bool preload = true);

	/**
	 * @brief Loads a sound from a file and returns the new SoundRef object.
	 *
	 * This method will never return NULL, even if the specified file does not exist
	 * or can not be read!
	 *
	 * This means you have to ensure the file exists before calling this method!
	 *
	 * @param filename The file to load from.
	 * @param preload Allows to preload the file.
	 * @return The new SoundRef
	 */
	SoundRef * loadSound(const char * filename, const bool preload = true);
	ShaderRef * loadShader(const char * filename, M_SHADER_TYPES type);

	// queue data updates
	/**
	 * @brief Appends a DataRef to the update queue.
	 *
	 * @param ref The DataRef object.
	 */
	void sendToUpdateQueue(DataRef * ref);

	/**
	 * @brief Appends a DataRef to the clear queue.
	 *
	 * @param ref The DataRef object.
	 */
	void sendToClearQueue(DataRef * ref);

	/**
	 * @brief Updates all DataRef's in the update queue.
	 *
	 * This will call DataRef::update for every object that is currently
	 * in the queue. The queue will be emptied so new requests can
	 * be handled.
	 */
	void updateQueueDatas(void);

	/**
	 * @brief Clear all DataRef's in the clear queue.
	 *
	 * This will call DataRef::clear for every object that is currently
	 * in the queue. The queue will be emptied so new requests can
	 * be handled.
	 */
	void clearQueueDatas(void);

	// scenes
	/**
	 * @brief Changes the current scene to the scene with the given ID.
	 *
	 * The scene will not change straight away but it will be merely requested to
	 * do so in one of the next frames.
	 *
	 * @param id The new scene ID
	 */
	void changeCurrentScene(unsigned int id); // does not change scene straight away, instead it requests to do it

	/**
	 * @brief Deletes the scene with the given ID.
	 *
	 * @param id The ID of the scene
	 */
	void deleteScene(unsigned int id);

	/**
	 * @brief Retrieves the index of the scene with the given name.
	 *
	 * The ID will be saved into the given pointer.
	 *
	 * @param name The name to search for
	 * @param index The pointer to an integer to save the ID
	 * @return Returns false if the scene can not be found.
	 */
	bool getSceneIndexByName(const char * name, unsigned int * index);

	/**
	 * @brief Creates a new scene and returns a pointer to it.
	 * @return The new scene
	 */
	Scene * addNewScene(void);

	/**
	 * @brief Retrieves the scene with the given name.
	 * @param name The name to search for
	 * @return The scene
	 */
	Scene * getSceneByName(const char * name);

	/**
	 * @brief Returns the current main scene.
	 * @return The current scene.
	 */
	Scene * getCurrentScene(void);

	/**
	 * @brief Changes the current scene.
	 * @param id The ID of the new scene.
	 */
	void setCurrentSceneId(unsigned int id);
	void setCurrentScene(Scene* s)
	{
		int oldId = m_currentSceneId;
		if(!getSceneIndexByName(s->getName(), &m_currentSceneId))
			m_currentSceneId = oldId;
	}

	/**
	 * @brief Returns the current scene ID
	 * @return The ID of the current scene.
	 */
	inline unsigned int getCurrentSceneId(void){ return m_currentSceneId; }

	/**
	 * @brief Returns the number of scenes.
	 * @return The number of scenes.
	 */
	inline unsigned int getScenesNumber(void){ return m_scenes.size(); }

	/**
	 * @brief Searches the scene with the given ID.
	 * @return The searched scene object.
	 */
	inline Scene * getSceneByIndex(unsigned int id){ return m_scenes[id]; }
};
}
#endif
