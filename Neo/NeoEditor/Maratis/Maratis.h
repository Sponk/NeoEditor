/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// Maratis.h
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


#ifndef _MARATIS_H
#define _MARATIS_H

#include <vector>
#include <NeoEngine.h>
#include <Plugin.h>
#include "../MRenderArray/MRenderArray.h"

namespace Neo
{
enum M_AXIS
{
	M_AXIS_NONE = 0,
	M_AXIS_X,
	M_AXIS_Y,
	M_AXIS_Z,
	M_AXIS_VIEW
};

enum M_ORIENTATION_MODE
{
	M_ORIENTATION_WORLD = 0,
	M_ORIENTATION_LOCAL
};

enum M_TRANSFORM_MODE
{
	M_TRANSFORM_MOUSE = 0,
	M_TRANSFORM_POSITION,
	M_TRANSFORM_ROTATION,
	M_TRANSFORM_SCALE
};


class Maratis
{
public:

	// constructor / destructor
	Maratis(void);
	~Maratis(void);

	// instance
	static Maratis * getInstance(void)
	{
		static Maratis m_instance;
		return &m_instance;
	}

private:

	// MEngine
	MSoundContext * m_soundContext;
	MRenderingContext * m_render;
	MPhysicsContext * m_physics;
	MScriptContext * m_script;
	MInputContext * m_input;
	MSystemContext * m_system;
	NeoGame * m_game;
	Level * m_level;
	Renderer * m_renderer;
	MPackageManager * m_packageManager;
	
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
	M_TRANSFORM_MODE m_transformMode;
	M_ORIENTATION_MODE m_orientationMode;
	M_AXIS m_currentAxis;
	MVector2 m_tMousePosition;
	MVector2 m_tCenterPosition;
	MVector2 m_tVectorDirection;
	MVector2 m_tOffsetDirection;
	MVector3 m_tPlane;

	// timeline
	int m_timelinePos;
	int m_timelineMin;
	int m_timelineMax;

	// selection
	MVector3 m_selectionCenter;
	MVector3 m_viewCenter;
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
	MDataManager m_meshManager;

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

public:

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
	void drawPositionAxis(M_AXIS axis, OCamera * camera, MMatrix4x4 * matrix, const bool viewTest = true);
	void drawScaleAxis(M_AXIS axis, OCamera * camera, MMatrix4x4 * matrix, const bool viewTest = true);
	void drawRotationCircle(M_AXIS axis, OCamera * camera, MMatrix4x4 * matrix, const bool zTest = true);
	void drawEditRotation(OCamera * camera);
	void drawEditPosition(OCamera * camera);
	void drawEditScale(OCamera * camera);
	void computeTransformDirection(OCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float distance, const MVector3 & axis);
	void computeTransformPlane(OCamera * camera, const MVector3 & position, const MVector3 & axis);
	void transformRotation(void);
	void transformPosition(void);
    void transformPosition(MVector2 delta);

	void transformScale(void);

    Object3d * getNearestMesh(Scene * scene, const MVector3 & rayO, const MVector3 & rayD, MVector3 * intersectPoint);
	Object3d * getNearestObject(Scene * scene, const MVector3 & rayO, const MVector3 & rayD, MVector3 * intersectPoint = NULL);
	M_AXIS selectEditRotation(OCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius);
	M_AXIS selectEditPosition(OCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius);
	M_AXIS selectEditScale(OCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius);

    void updateCurrentAxis();

	// file browser
	static void okAddEntity(const char * filename);
	static void okAddSound(const char * filename);
	static void okAddFont(const char * filename);
	static void okNewProject(const char * filename);
	static void okLoadProject(const char * filename);
	static void okLoadLevel(const char * filename);
	static void okSaveAs(const char * filename);
	static void okImportExternal(const char * filename);
	
public:

	// temp dir
	const char * getTempDir(void){ return m_tempDir; }

	// renderer
	void changeRenderer(const char * name);
	
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
	inline void setTransformMode(M_TRANSFORM_MODE transformMode){ m_transformMode = transformMode; }
	inline void setOrientationMode(M_ORIENTATION_MODE orientationMode){ m_orientationMode = orientationMode; }
	inline M_TRANSFORM_MODE getTransformMode(void){ return m_transformMode; }
	inline M_ORIENTATION_MODE getOrientationMode(void){ return m_orientationMode; }
	inline void clearCurrentAxis(void){ m_currentAxis = M_AXIS_NONE; }
	void transformSelectedObjects(void);

	// selection
	inline MVector3 * getSelectionCenter(void){ return &m_selectionCenter; }

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
	
	// main loops
	static void logicLoop(void);
	static void graphicLoop(void);
};
}
#endif
