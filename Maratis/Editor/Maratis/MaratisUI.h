/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MaratisUI.h
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


#ifndef _MARATIS_UI_H
#define _MARATIS_UI_H

#include <string>
#include <vector>
#include <MEngine.h>
#include <MGui.h>

#include "Maratis.h"


class MaratisUI
{
public:

	// constructor / destructor
	MaratisUI(void);
	virtual ~MaratisUI(void);

	// instance
	static MaratisUI * getInstance(void)
	{
		static MaratisUI m_instance;
		return &m_instance;
	};

public:

	// entity
	bool m_editGhost;
	bool m_editInvisible;
	bool m_editPhysics;
	bool m_editConstraint;
	float m_editMass;
	float m_editFriction;
	float m_editRestitution;
	float m_editLinearDamping;
	float m_editAngularDamping;
	float m_editAngularFactor;

	// camera
	bool m_editOrtho;
	bool m_editFog;
	float m_editFov;
	float m_editClipNear;
	float m_editClipFar;
	float m_editFogDistance;
	MVector3 m_editCameraColor;

	// light
	bool m_editSpot;
	bool m_editShadow;
	float m_editRadius;
	float m_editIntensity;
	float m_editSpotAngle;
	float m_editSpotExponent;
	float m_editShadowBias;
	float m_editShadowBlur;
	MVector3 m_editLightColor;
	unsigned int m_editShadowQuality;

	// sound
	bool m_editLoop;
	bool m_editRelative;
	float m_editPitch;
	float m_editGain;
	float m_editSndRadius;
	float m_editRolloff;

	// text
	float m_editSize;
	MString m_editText;
	MVector4 m_editTextColor;

	// scene
	MVector3 m_editGravity;
	
	// transform
	MString		m_editParent;
	MVector3	m_editPosition;
	MVector3	m_editRotation;
	MVector3	m_editScale;
    
    //shortcuts
    struct Shortcut
    {
        unsigned int key1;
        unsigned int key2;
        unsigned int key3;
    };
    
    Shortcut m_scSave;
    Shortcut m_scUndo;
    Shortcut m_scRedo;
    Shortcut m_scLoadLevel;
    Shortcut m_scQuit;
    Shortcut m_scDuplicate;
    Shortcut m_scSelectAll;
    Shortcut m_scOrthogonalView;
    Shortcut m_scFaceView;
    Shortcut m_scRightView;
    Shortcut m_scTopView;
    Shortcut m_scBottomView;
    Shortcut m_scTransformMouse;
    Shortcut m_scTransformPosition;
    Shortcut m_scTransformRotation;
    Shortcut m_scTransformScale;
    Shortcut m_scMultipleSelection;
    Shortcut m_scFocusSelection;
    Shortcut m_scDelete;
    Shortcut m_scDelete2;
    Shortcut m_scPanView;
    
    map<string, unsigned int> m_keys;
    map<string, struct Shortcut*> m_shortcuts;
    
    int m_lastInput;

private:

	// behaviors
	unsigned int m_editedBehavior;

	// update edit
	bool m_needToUpdateEdit;

	// GUI variables
	bool m_play;
	bool m_pause;
	bool m_mouse;
	bool m_position;
	bool m_rotation;
	bool m_scale;
	int m_worldMenu;

	MGuiWindow * m_headWin;				// file, edit, add...
	MGuiWindow * m_cameraWin;			// camera
	MGuiWindow * m_editTopWin;			// transform / properties / behaviors
	MGuiWindow * m_editWin;				// position, rotation...
	MGuiWindow * m_textWin;				// script...
	MGuiWindow * m_timeLeftWin;			// timeline min edit
	MGuiWindow * m_timeRightWin;		// timeline max edit, play, pause, add key...
	MGuiWindow * m_timelineWin;			// timeline bar
	MGuiWindow * m_keysWin;				// keys edit

	MGuiWindow * m_popupWin;			// popup

	MGuiWindow * m_view;				// view
	MGuiWindow * m_viewProp;			// view properties

	MVector3 m_guiColor;				// GUI color

	// view props
	MGuiMenu * m_viewMenu;

	// time
	MGuiSlide * m_timelineSlide;
	MGuiEditText * m_timePosEdit;

	// top buttons
	MGuiButton * m_mouseButton;
	MGuiButton * m_positionButton;
	MGuiButton * m_rotationButton;
	MGuiButton * m_scaleButton;

	// tabs
	MGuiImage * m_tabImage;
	MGuiButton * m_tabTransform;
	MGuiButton * m_tabProperties;
	MGuiButton * m_tabBehavior;
    MGuiButton * m_tabObjects;

	MGuiImage * m_sceneProperties;
	MGuiImage * m_projProperties;
	MGuiImage * m_objectListImage;

	// renderer
	MGuiMenu * m_rendererMenu;
	
	// info text
	MGuiText * m_infoText;

	// text
	MGuiText * m_text;
	MGuiText * m_tabText;

	// transform info
	MGuiImage * m_positionImage;
	MGuiImage * m_rotationImage;
	MGuiImage * m_scaleImage;

	// object
	MGuiEditText * m_objectEdit;

	// anim id
	MGuiText * m_animIdText;

	// scene
	MGuiEditText * m_sceneEdit;
	MGuiMenu * m_sceneMenu;

	// edited object
	bool m_editObject;
	MObject3d * m_editedObject;

	// special event
	int m_specialEvent;
	string m_specialEventInfo;
	
	// file browser
	MGuiFileBrowser * m_fileBrowser;
	static void fileBrowserEvents(MGuiFileBrowser * fileBrowser, MGUI_FILE_BROWSER_EVENT_TYPE event);
	void (* m_fileBrowserFuncPointer)(const char * filename);
	
	static void okLoadFont(const char * filename);
	static void okLoadTexture(const char * filename);
	static void okLoadScript(const char * filename);
	
private:

	// edit variable
	void addVariableName(MGuiWindow * window, const char * variableName, MVector2 * position);
	void addValue(MGuiWindow * window, const char * valueName, M_VAR_TYPE varType, void * pointer, MVector2 * position);

	// add text / buttons
	void addTextButtonToMenu(MGuiMenu * menu, const char * text, MVector2 position, void (* buttonPointerEvent)(MGuiButton * button, MGuiEvent * guiEvents));
	void addTextToMenu(MGuiMenu * menu, const char * text, MVector2 position);
	MGuiButton * addButtonToWindow(MGuiWindow * window, const char * texture, MVector2 position, void (* buttonPointerEvent)(MGuiButton * button, MGuiEvent * guiEvents));
    
    //load settings
    void loadSettings(void);

	// clear
	void clear(void);

	// gui
	void createGUI(void);

public:

	// file browser
	void openFileBrowser(const char * startPath, const char * startFile, const char * okName, void (* functionPointer)(const char * filename));
	
	// end game
	void endGame(void);
	
    //check shortcut
    bool isShortcutEngaged(struct Shortcut * sc, bool checkLastInput = true);
    
    // is playing
	inline bool isPlaying(void){ return m_play; }

	// special events
	void setSpecialEvent(int specialEvent, const char * info);
	void specialEvents(void);

	// anim id
	void updateAnimIdText(MOEntity * entity);

	// behavior
	inline void setEditedBehavior(const unsigned int behavior){ m_editedBehavior = behavior; }
	inline unsigned int getEditedBehavior(void){ return m_editedBehavior; }

	// edit
	void setEditedObject(MObject3d * object){ m_editedObject = object; }
	MObject3d * getEditedObject(void){ return m_editedObject; }
	void doEditObject(MObject3d * object);
	void editObject(MObject3d * object);
	void editScene(void);
	void editProject(void);
	inline void setNeedToUpdateEdit(bool update){ m_needToUpdateEdit = update; }
	inline bool needToUpdateEdit(void){ return m_needToUpdateEdit; }

	// transform
	void drawInfoTransform(void);
	void setTransformMode(M_TRANSFORM_MODE mode);

	// GUI
	void resizeGUI(void);
	inline MVector3 * getGUIColor(void){ return &m_guiColor; }

	inline MGuiWindow * getHeadWin(void){ return m_headWin; }
	inline MGuiWindow * getEditWin(void){ return m_editWin; }
	inline MGuiWindow * getTextWin(void){ return m_textWin; }
	inline MGuiWindow * getTimeLeftWin(void){ return m_timeLeftWin; }
	inline MGuiWindow * getTimeRightwin(void){ return m_timeRightWin; }
	inline MGuiWindow * getTimelineWin(void){ return m_timelineWin; }
	inline MGuiWindow * getkeysWin(void){ return m_keysWin; }

	inline MGuiWindow * getPopupWin(void){ return m_popupWin; }

	inline MGuiWindow * getView(void){ return m_view; }
	inline MGuiWindow * getViewProp(void){ return m_viewProp; }

	inline MGuiSlide * getTimelineSlide(void){ return m_timelineSlide; }
	inline MGuiEditText * getTimePosEdit(void){ return m_timePosEdit; }

	inline MGuiEditText * getSceneEdit(void){ return m_sceneEdit; }
	inline MGuiMenu * getSceneMenu(void){ return m_sceneMenu; }

	inline MGuiEditText * getObjectEdit(void){ return m_objectEdit; }

	inline MGuiMenu * getRendererMenu(void){ return m_rendererMenu; }
	
	// view menu
	void updateViewMenu(void);

	// scene menu
	void updateSceneMenu(void);

	// info text
	void drawInfoText(void);
	MGuiText * getInfoText(void){ return m_infoText; }

	// draw text
	void drawText(int x, int y, const char * text, const MVector4 & color);

	// popup
	void closePopupButton(void);
	void setPopupButton(const char * text, void (* buttonPointerEvent)(MGuiButton * button, MGuiEvent * guiEvents));

	// events
	static void windowEvents(MWinEvent * windowEvents);

	static void cameraWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void headWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void editTopWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void editWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void textWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void timeLeftWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void timeRightWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void timelineWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void keysWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void viewPropEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	static void viewEvents(MGuiWindow * window, MGuiEvent * guiEvents);

	static void popupWinEvents(MGuiWindow * window, MGuiEvent * guiEvents);
	
	static void loadTextureEvents(MGuiButton * button, MGuiEvent * guiEvents);
	
	
	// view props
	static void viewMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents);

	// timeline slide
	static void timelineSlideEvents(MGuiSlide * slide, MGuiEvent * guiEvents);

	// animation id
	static void leftAnimIdEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void rightAnimIdEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// physics
	static void collisionShapeEvents(MGuiMenu * menu, MGuiEvent * guiEvents);

	// object
	static void editEvents(MGuiEditText * edit, MGuiEvent * guiEvents);
	static void objectEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents);
	static void okDeleteObjectsEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// dynamic variables
	static void dVarButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// time edit
	static void timeLeftEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents);
	static void timeRightEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents);
	static void timePosEditEvents(MGuiEditText * edit, MGuiEvent * guiEvents);

	// time buttons
	static void playButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void pauseButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void keyButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// top buttons events
	static void mouseButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void positionButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void rotationButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void scaleButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);

	static void worldButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void localButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);

	static void cameraButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void entityButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void backgroundButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void particlesButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void lightButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void soundButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void textButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void shapeButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void levelButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void sceneButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void gameButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// tab events
	static void tabTransformEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void tabPropertiesEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void tabBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents);
    static void tabObjectsEvents(MGuiButton * button, MGuiEvent * guiEvents);
    
    // object button events
    static void objectButtonEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// text
	static void textAlignMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents);

	// behavior
	static void okDeleteBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void upBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void downBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void deleteBehaviorEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void behaviorMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents);
	
	// scene
	static void newSceneEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void sceneNameEvents(MGuiEditText * edit, MGuiEvent * guiEvents);
	static void sceneMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents);
	static void deleteSceneEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void okDeleteSceneEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void dataMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents);

	// menu buttons
	static void quitEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void okQuitEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void newLevelEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void okNewLevelEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void openLevelEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void saveEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void saveAsEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void newProjectEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void openProjectEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void importExternalEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void publishEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// edit menu buttons
	static void activeEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void undoEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void redoEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void selectAllEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void selectSameMeshEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void duplicateEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void deleteEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void linkEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void unlinkEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// script buttons
	static void loadScriptEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void deleteScriptEvents(MGuiButton * button, MGuiEvent * guiEvents);
	static void editScriptEvents(MGuiButton * button, MGuiEvent * guiEvents);

	// font button
	static void loadFontEvents(MGuiButton * button, MGuiEvent * guiEvents);
	
	// renderer
	static void rendererMenuEvents(MGuiMenu * menu, MGuiEvent * guiEvents);
};


#endif