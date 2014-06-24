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
	MOCamera * getCurrentCamera(void);

	// current frame
	inline void setCurrentFrame(int currentFrame){ m_currentFrame = currentFrame; }
	inline int getCurrentFrame(void){ return m_currentFrame; }

	// get objects
	MObject3d * getObjectByName(const char * name);
	MOLight * getLightByName(const char * name);
	MOCamera * getCameraByName(const char * name);
	MOEntity * getEntityByName(const char * name);
	MOSound * getSoundByName(const char * name);
	MOText * getTextByName(const char * name);
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
	void update(void);

	// behaviors
	void updateObjectsBehaviors(void);
	void drawObjectsBehaviors(void);
	
	// update objects
	void updateObjectsMatrices(void);

	// draw
	void draw(MOCamera * camera);
};


#endif
