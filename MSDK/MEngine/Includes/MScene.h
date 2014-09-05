/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MScene.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#ifndef _M_SCENE_H
#define _M_SCENE_H

// data modes
enum M_DATA_MODES
{
	M_DATA_STATIC = 0,
	M_DATA_DYNAMIC,
	M_DATA_STREAM
};

// MScene
/**
 * @brief The MScene class represents a scene in a Maratis level.
 *
 * One level can have multiple independent scenes with objects and scripts.
 * You can use this to overlay a scene over another to create ingame GUI or similar.
 *
 */
class M_ENGINE_EXPORT MScene
{
public:

	// constructor / destructor
	MScene(void);
	~MScene(void);

private:

	// name
	MString m_name;

	// data mode
	M_DATA_MODES m_dataMode;

	// script
	MString m_scriptFilename;

	// objects pointers
	vector <MObject3d *> m_objects;

	// objects
	vector <MOCamera *> m_cameras;
	vector <MOLight *> m_lights;
	vector <MOEntity *> m_entities;
	vector <MOSound *> m_sounds;
	vector <MOText *> m_texts;

	// current frame
	int m_currentFrame;

	// current camera
	unsigned int m_currentCamera;

	// gravity
	MVector3 m_gravity;

    // Ambient light
    MVector3 m_ambientLight;

public:

	// name
    /**
     * @brief Sets the scene name.
     *
     * The scene name is a property by which you can find a particular scene.
     *
     * @see MLevel::getSceneByName
     *
     * @param name The new name which will be copied to an internal buffer.
     */
	void setName(const char * name);

    /**
     * @brief Returns the name of the scene.
     * @return The scene name.
     */
	inline const char * getName(void){ return m_name.getSafeString(); }

    inline MVector3 getAmbientLight() { return m_ambientLight; }
    inline void setAmbientLight(MVector3 light) { m_ambientLight = light; }

	// data mode
	inline void setDataMode(M_DATA_MODES dataMode){ m_dataMode = dataMode; }
	inline M_DATA_MODES getDataMode(void){ return m_dataMode; }

	// script
    /**
     * @brief Sets the scenes script that will be executed.
     *
     * Sets the path to a script. Those script can contain game logics.
     *
     * @param scriptFilename
     */
	void setScriptFilename(const char * scriptFilename);

    /**
     * @brief Returns the filename of the script that is currently attached to this scene.
     * @return The path to the script.
     */
	inline const char * getScriptFilename(void){ return m_scriptFilename.getData(); }

	// gravity
    /**
     * @brief Sets the gravitational pull in this scene to the given value.
     *
     * This new gravity is stored in a MVector3.
     *
     * Example:
     * \code
     * scene.setGravity(MVector3(0.0f, 0.0f, -9.81f));
     * \endcode
     *
     * @param gravity
     */
	inline void setGravity(const MVector3 & gravity){ m_gravity = gravity; }

    /**
     * @brief Returns the current gravity in a MVector3
     * @return The gravity vector.
     */
	inline MVector3 getGravity(void) const { return m_gravity; }

	// sounds
	void playLoopSounds(void);
	void stopAllSounds(void);

	// delete object
    /**
     * @brief Removes the given object from this scene.
     * @param object The object to be removed.
     */
	void deleteObject(MObject3d * object);

	// add objects
    /**
     * @brief Creates a new camera, adds it to the scene and returns it.
     *
     * @see MCamera
     *
     * @return The new camera.
     */
	MOCamera * addNewCamera(void);

    /**
     * @brief Copies the camera and adds the new instance to the scene.
     * @param camera The camera to copy.
     * @return The new copy of the camera.
     */
	MOCamera * addNewCamera(const MOCamera & camera);

    /**
     * @brief Creates a new light source and adds it to the scene.
     *
     * @see MOLight
     *
     * @return The new light source.
     */
	MOLight * addNewLight(void);

    MObject3d * addNewGroup(void);
    MObject3d * addNewGroup(const MObject3d & object);

    /**
     * @brief Copies the light and adds the new instance to the scene.
     * @param light The light to copy from.
     * @return The new copy.
     */
	MOLight * addNewLight(const MOLight & light);
	MOEntity * addNewEntity(MMeshRef * meshRef);
	MOEntity * addNewEntity(const MOEntity & entity);
	MOSound * addNewSound(MSoundRef * soundRef);
	MOSound * addNewSound(const MOSound & sound);
	MOText * addNewText(MFontRef * fontRef);
	MOText * addNewText(const MOText & text);

	// get objects number
    /**
     * @brief Returns the number of objects in the scene.
     * @return Number of objects
     */
	inline unsigned int getObjectsNumber(void){ return m_objects.size(); }

    /**
     * @brief Returns the number of cameras in the scene.
     * @return Number of cameras
     */
	inline unsigned int getCamerasNumber(void){ return m_cameras.size(); }

    /**
     * @brief Returns the number of lights in the scene.
     * @return Number of lights
     */
	inline unsigned int getLightsNumber(void){ return m_lights.size(); }

    /**
     * @brief Returns the number of entities in the scene.
     * @return Number of entities
     */
	inline unsigned int getEntitiesNumber(void){ return m_entities.size(); }

    /**
     * @brief Returns the number of sounds in the scene.
     * @return Number of sounds
     */
	inline unsigned int getSoundsNumber(void){ return m_sounds.size(); }

    /**
     * @brief Returns the number of text objects in the scene.
     * @return Number of text objects
     */
	inline unsigned int getTextsNumber(void){ return m_texts.size(); }

	// current camera
	inline void setCurrentCameraId(unsigned int id){ m_currentCamera = id; }
	inline unsigned int getCurrentCameraId(void){ return m_currentCamera; }

    /**
     * @brief Returns the currently active camera from this scene.
     *
     * @see MOCamera
     *
     * @return The current camera.
     */
    MOCamera * getCurrentCamera(void);

	// current frame
	inline void setCurrentFrame(int currentFrame){ m_currentFrame = currentFrame; }
	inline int getCurrentFrame(void){ return m_currentFrame; }

	// get objects
    /**
     * @brief Returns the object with the given name or NULL if it is not found.
     *
     * @see MObject3d
     *
     * @param name The objects name.
     * @return The object or NULL.
     */
	MObject3d * getObjectByName(const char * name);

    /**
     * @brief Returns the light with the given name or NULL if it is not found.
     *
     * @see MOLight
     *
     * @param name The lights name.
     * @return The light or NULL.
     */
	MOLight * getLightByName(const char * name);

    /**
     * @brief Returns the camera with the given name or NULL if it is not found.
     *
     * @see MOCamera
     *
     * @param name The cameras name.
     * @return The camera or NULL.
     */
	MOCamera * getCameraByName(const char * name);

    /**
     * @brief Returns the entity with the given name or NULL if it is not found.
     *
     * @see MOEntity
     *
     * @param name The entities name.
     * @return The entity or NULL.
     */
	MOEntity * getEntityByName(const char * name);

    /**
     * @brief Returns the sound with the given name or NULL if it is not found.
     *
     * @see MOSound
     *
     * @param name The sounds name.
     * @return The sound or NULL.
     */
	MOSound * getSoundByName(const char * name);

    /**
     * @brief Returns the text object with the given name or NULL if it is not found.
     *
     * @see MOText
     *
     * @param name The objects name.
     * @return The object or NULL.
     */
	MOText * getTextByName(const char * name);

    /**
     * @brief Searches the object with the given name and copies the ID.
     *
     * The ID will be copied to the location where the pointer id points to.
     *
     * Example:
     * \code
     * unsigned int id;
     * scene->getObjectIndex("object", &id);
     * \endcode
     *
     * @param name The name of the object.
     * @param id Pointer to an unsigned int to retrieve the data.
     * @return Returns \b true if the object exists and \b false if it doesn't.
     */
	bool getObjectIndex(const char * name, unsigned int * id);

	inline MObject3d * getObjectByIndex(unsigned int index){ return m_objects[index]; }
	inline MOLight * getLightByIndex(unsigned int index){ return m_lights[index]; }
	inline MOCamera * getCameraByIndex(unsigned int index){ return m_cameras[index]; }
	inline MOEntity * getEntityByIndex(unsigned int index){ return m_entities[index]; }
	inline MOSound * getSoundByIndex(unsigned int index){ return m_sounds[index]; }
	inline MOText * getTextByIndex(unsigned int index){ return m_texts[index]; }

	// begin / end
	void begin(void);
	void end(void);

	// physics
	void prepareCollisionShape(MOEntity * entity);
	void prepareCollisionObject(MOEntity * entity);
	void prepareConstraints(MOEntity * entity);
	void preparePhysics(void);
	void updatePhysics(void);

	// update
    /**
     * @brief Updates the scene and all objects in it.
     */
	void update(void);

	// behaviors
    /**
     * @brief Updates all behaviors of objects in this scene.
     * @see MBehavior
     */
	void updateObjectsBehaviors(void);

    /**
     * @brief Call draw for all behaviors in this scene.
     * @see MBehavior
     */
	void drawObjectsBehaviors(void);
	
	// update objects
	void updateObjectsMatrices(void);

	// draw
    /**
     * @brief Renders the scene to the screen.
     *
     * This method takes the camera and renders everything from this point of view to the screen/buffer.
     * This is done by using a subclass of MRenderingContext to render.
     *
     * @see MRenderingContext
     *
     * @param camera The camera to use.
     */
	void draw(MOCamera * camera);
};


#endif
