//========================================================================
//  Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef _EDITOR_BACKEND_H
#define _EDITOR_BACKEND_H

#include <vector>
#include <NeoEngine.h>
#include <Plugin.h>

#include "../RenderArray/RenderArray.h"
#include "../PluginScript/PluginScript.h"


namespace Neo
{
enum AXIS
{
	AXIS_NONE = 0,
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_VIEW
};

enum ORIENTATION_MODE
{
	ORIENTATION_WORLD = 0,
	ORIENTATION_LOCAL
};

enum TRANSFORM_MODE
{
	TRANSFORM_MOUSE = 0,
	TRANSFORM_POSITION,
	TRANSFORM_ROTATION,
	TRANSFORM_SCALE
};


class EditorBackend
{
public:

	// constructor / destructor
	EditorBackend(void);
	~EditorBackend(void);

	// instance
	static EditorBackend * getInstance(void)
	{
		static EditorBackend m_instance;
		return &m_instance;
	}

private:

	// MEngine
	SoundContext * m_soundContext;
	RenderingContext * m_render;
	PhysicsContext * m_physics;
	ScriptContext * m_script;
	InputContext * m_input;
	SystemContext * m_system;
	NeoGame * m_game;
	Level * m_level;
	Renderer * m_renderer;
	PackageManager * m_packageManager;
	
	// temp dir
	char m_tempDir[256];

	// current
	char m_currentProject[256];
	char m_currentLevel[256];
    char m_windowTitle[256];

    bool m_titleChanged;

	// undo
	bool m_firstUndo;
	unsigned int m_maxUndo;
	unsigned int m_undo;
	unsigned int m_undoNumber;
	unsigned int m_undoLevel;

	// transform
	TRANSFORM_MODE m_transformMode;
	ORIENTATION_MODE m_orientationMode;
	AXIS m_currentAxis;
	Vector2 m_tMousePosition;
	Vector2 m_tCenterPosition;
	Vector2 m_tVectorDirection;
	Vector2 m_tOffsetDirection;
	Vector3 m_tPlane;

	// timeline
	int m_timelinePos;
	int m_timelineMin;
	int m_timelineMax;

	// selection
	Vector3 m_selectionCenter;
	Vector3 m_viewCenter;
	bool m_selectionLock;

	OEntity * m_sphereEntity;
	OEntity * m_coneEntity;
	OEntity * m_cubeEntity;
	OEntity * m_planeEntity;
	OEntity * m_xEntity;
	OEntity * m_yEntity;
	OEntity * m_zEntity;
	OEntity * m_xcircleEntity;
	OEntity * m_ycircleEntity;
	OEntity * m_zcircleEntity;
	OEntity * m_lightEntity;
	OEntity * m_cameraEntity;
	OEntity * m_soundEntity;

	// mesh manager
	DataManager m_meshManager;

	// user vues
	OCamera m_perspectiveVue;

	// selected objects
	std::vector <Object3d *> m_selectedObjects;

	// plugins
	std::vector <Neo::Plugin *> m_plugins;

	// game plugin
	Neo::Plugin * m_gamePlugin;

	// empty text
	OText m_emptyText;

	// Snap to grid
	bool m_snapToGrid;
	float m_snapDistance;

	// Lua plugins
	std::vector<PluginScript*> m_editorPlugins;
	PluginScript* m_inputMethod;

	bool vectorContains(std::string name);

public:

	// Lua plugins
	void loadPluginsFrom(const char* src);
	void initPlugins();
	void selectInputMethod(const char* name);
	PluginScript* getPlugin(int idx) { return m_editorPlugins[idx]; }

	PluginScript* getInputMethod() { return m_inputMethod; }
	void setInputMethod(PluginScript* s) { m_inputMethod = s; }

	size_t getNumPlugins() { return m_editorPlugins.size(); }
	
	
	void enableSnapToGrid() { m_snapToGrid = true; }
	void disableSnapToGrid() { m_snapToGrid = false; }

	bool isSnapToGridEnabled() { return m_snapToGrid; }
	void setSnapDistance(float d) { m_snapDistance = d; }
	float getSnapDistance() { return m_snapDistance; }

	// title
	void updateTitle(const char * additional = NULL);

	// names
	void getNewObjectName(const char * objectName, char * name);

	// editor meshs
	MeshRef * loadEditorMesh(const char * filename);

	// start
	void start(void);

    const char* getWindowTitle() { return m_windowTitle; }
    bool hasTitleChanged() { return m_titleChanged; }
    void setTitleChanged(bool changed) { m_titleChanged = changed; }

	void drawInvisibleEntity(OEntity * entity);
	void drawTriangles(Mesh * mesh);
	void drawLight(void);
	void drawCamera(void);
	void drawSound(void);
	void drawArmature(OEntity * entity);
	void drawBoundingBox(Box3d * box);
	void drawGrid(Scene * scene);
	void drawPositionAxis(AXIS axis, OCamera * camera, Matrix4x4 * matrix, const bool viewTest = true);
	void drawScaleAxis(AXIS axis, OCamera * camera, Matrix4x4 * matrix, const bool viewTest = true);
	void drawRotationCircle(AXIS axis, OCamera * camera, Matrix4x4 * matrix, const bool zTest = true);
	void drawEditRotation(OCamera * camera);
	void drawEditPosition(OCamera * camera);
	void drawEditScale(OCamera * camera);
	void computeTransformDirection(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, float distance, const Vector3 & axis);
	void computeTransformPlane(OCamera * camera, const Vector3 & position, const Vector3 & axis);
	void transformRotation(void);
	void transformPosition(void);
    void transformPosition(Vector2 delta);

	void transformScale(void);

    Object3d * getNearestMesh(Scene * scene, const Vector3 & rayO, const Vector3 & rayD, Vector3 * intersectPoint);
	Object3d * getNearestObject(Scene * scene, const Vector3 & rayO, const Vector3 & rayD, Vector3 * intersectPoint = NULL);
	AXIS selectEditRotation(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, float radius);
	AXIS selectEditPosition(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, float radius);
	AXIS selectEditScale(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, float radius);

    void updateCurrentAxis();

	// file browser
	void okAddEntity(const char * filename);
	void okAddSound(const char * filename);
	void okAddFont(const char * filename);
	void okNewProject(const char * filename);
	void okLoadProject(const char * filename);
	void okLoadLevel(const char * filename);
	void okSaveAs(const char * filename);
	void okImportExternal(const char * filename);
	
public:

	// temp dir
	const char * getTempDir(void){ return m_tempDir; }

	// renderer
	void changeRenderer(const char * name);

	// Used by tests
	void setRenderingContext(RenderingContext* context) { m_render = context; }
	
	// undo
	void autoSave(void);
	void undo(void);
	void redo(void);
	void clearUndo(void);

	// init
	void initVue(void);
    void initRenderer(void);

	// game plugin
	void loadGamePlugin(void);

	// restart
	void restart(void);

	// clear
	void clear(void);

	// add objects
	void addEntity(void);
	void addCamera(void);
	void addLight(void);
	void addSound(void);
	void addText(void);
	void addGroup(void);

	// project load/save
	void newProject(void);
	void loadProject(void);
	void loadProject(const char * filename);
	void newLevel(void);
	bool loadLevel(const char * filename);
	void loadLevel(void);
	void save(void);
	void saveAs(void);
	
	// import
	void importExternal(void);

	// delete selected objects
	void deleteSelectedObjects(void);

	// duplicate selected objects
	void duplicateSelectedObjects(void);
	Object3d* duplicateObject(Object3d *object);
	
	// link
	void linkSelectedObjects(void);
	void unlinkSelectedObjects(void);
	void linkTwoObjects(Object3d *parent, Object3d *child);
	void unlinkTwoObjects(Object3d *parent, Object3d *child);

	// selected objects
	void activeSelection(void);
	void selectAll(void);
	void focusSelection(void);
	void selectSameMesh(void);
	void addSelectedObject(Object3d * object);
	bool isObjectSelected(Object3d * object);
	inline void clearSelectedObjects(void){ if(!m_selectionLock) m_selectedObjects.clear(); }
	inline unsigned int getSelectedObjectsNumber(void){ return m_selectedObjects.size(); }
	inline Object3d * getSelectedObjectByIndex(unsigned int id){ return m_selectedObjects[id]; }
	void lockSelection() { m_selectionLock = true; }
	void unlockSelection() { m_selectionLock = false; }

    // Level
    const char* getCurrentLevel() { return m_currentLevel; }
    // TODO: Boundary checks!
    void setCurrentLevel(const char* path) { strcpy(m_currentLevel, path); }

	// play
	bool isPlaying(void);

	// timeline
	inline void setTimelinePos(int pos){ m_timelinePos = pos; }
	inline void setTimelineMin(int min){ m_timelineMin = min; }
	inline void setTimelineMax(int max){ m_timelineMax = max; }

	inline int getTimelinePos(void){ return m_timelinePos; }
	inline int getTimelineMin(void){ return m_timelineMin; }
	inline int getTimelineMax(void){ return m_timelineMax; }

	inline int * getTimelinePosPtr(void){ return &m_timelinePos; }
	inline int * getTimelineMinPtr(void){ return &m_timelineMin; }
	inline int * getTimelineMaxPtr(void){ return &m_timelineMax; }

	// trasform mode
	inline void setTransformMode(TRANSFORM_MODE transformMode){ m_transformMode = transformMode; }
	inline void setOrientationMode(ORIENTATION_MODE orientationMode){ m_orientationMode = orientationMode; }
	inline TRANSFORM_MODE getTransformMode(void){ return m_transformMode; }
	inline ORIENTATION_MODE getOrientationMode(void){ return m_orientationMode; }
	inline void clearCurrentAxis(void){ m_currentAxis = AXIS_NONE; }
	void transformSelectedObjects(void);

	// selection
	inline Vector3 * getSelectionCenter(void){ return &m_selectionCenter; }

	// user vues
	void rotateCurrentVue(void);
	void panCurrentVue(void);
	void zoomCurrentVue(void);
	void switchCurrentVueMode(void);
	void changeCurrentVue(int vue);
	inline OCamera * getPerspectiveVue(void){ return &m_perspectiveVue; }

	// drawing
	void drawCamera(Scene * scene, OCamera * camera);
	void drawMainView(Scene * scene);

	// selection
    void selectObjectsInMainView(Scene* scene, bool multipleSelection);
	void updateSelectionCenter(void);
	void updateViewCenter(void);

	// publish
	void publish(void);
	void publish(const char* dest, const char* exec);
	
	// main loops
	void logicLoop(void);
	void graphicLoop(void);
};
}
#endif
