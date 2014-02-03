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
#include <MEngine.h>
#include <MPlugin/MPlugin.h>
#include "../MRenderArray/MRenderArray.h"


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
	MGame * m_game;
	MLevel * m_level;
	MRenderer * m_renderer;
	MPackageManager * m_packageManager;
	
	// temp dir
	char m_tempDir[256];

	// current
	char m_currentProject[256];
	char m_currentLevel[256];

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

	MOEntity * m_sphereEntity;
	MOEntity * m_coneEntity;
	MOEntity * m_cubeEntity;
	MOEntity * m_planeEntity;
	MOEntity * m_xEntity;
	MOEntity * m_yEntity;
	MOEntity * m_zEntity;
	MOEntity * m_xcircleEntity;
	MOEntity * m_ycircleEntity;
	MOEntity * m_zcircleEntity;
	MOEntity * m_lightEntity;
	MOEntity * m_cameraEntity;
	MOEntity * m_soundEntity;

	// mesh manager
	MDataManager m_meshManager;

	// user vues
	MOCamera m_perspectiveVue;

	// selected objects
	std::vector <MObject3d *> m_selectedObjects;

	// plugins
	std::vector <MPlugin *> m_plugins;

	// game plugin
	MPlugin * m_gamePlugin;

	// empty text
	MOText m_emptyText;

private:

	// title
	void updateTitle(const char * additional = NULL);

	// names
	void getNewObjectName(const char * objectName, char * name);

	// editor meshs
	MMeshRef * loadEditorMesh(const char * filename);

	// start
	void start(void);

	void drawInvisibleEntity(MOEntity * entity);
	void drawTriangles(MMesh * mesh);
	void drawLight(void);
	void drawCamera(void);
	void drawSound(void);
	void drawArmature(MOEntity * entity);
	void drawBoundingBox(MBox3d * box);
	void drawGrid(MScene * scene);
	void drawPositionAxis(M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, const bool viewTest = true);
	void drawScaleAxis(M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, const bool viewTest = true);
	void drawRotationCircle(M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, const bool zTest = true);
	void drawEditRotation(MOCamera * camera);
	void drawEditPosition(MOCamera * camera);
	void drawEditScale(MOCamera * camera);
	void computeTransformDirection(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float distance, const MVector3 & axis);
	void computeTransformPlane(MOCamera * camera, const MVector3 & position, const MVector3 & axis);
	void transformRotation(void);
	void transformPosition(void);
	void transformScale(void);
	MObject3d * getNearestMesh(MScene * scene, const MVector3 & rayO, const MVector3 & rayD, MVector3 * intersectPoint);
	MObject3d * getNearestObject(MScene * scene, const MVector3 & rayO, const MVector3 & rayD, MVector3 * intersectPoint = NULL);
	M_AXIS selectEditRotation(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius);
	M_AXIS selectEditPosition(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius);
	M_AXIS selectEditScale(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius);

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
	void linkTwoObjects(MObject3d *parent, MObject3d *child);
	void unlinkTwoObjects(MObject3d *parent, MObject3d *child);

	// selected objects
	void activeSelection(void);
	void selectAll(void);
	void focusSelection(void);
	void selectSameMesh(void);
	void addSelectedObject(MObject3d * object);
	bool isObjectSelected(MObject3d * object);
	inline void clearSelectedObjects(void){ m_selectedObjects.clear(); }
	inline unsigned int getSelectedObjectsNumber(void){ return m_selectedObjects.size(); }
	inline MObject3d * getSelectedObjectByIndex(unsigned int id){ return m_selectedObjects[id]; }

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
	inline MOCamera * getPerspectiveVue(void){ return &m_perspectiveVue; }

	// drawing
	void drawCamera(MScene * scene, MOCamera * camera);
	void drawMainView(MScene * scene);

	// selection
	void selectObjectsInMainView(MScene * scene);
	void updateSelectionCenter(void);
	void updateViewCenter(void);

	// publish
	void publish(void);
	
	// main loops
	static void logicLoop(void);
	static void graphicLoop(void);
};

#endif