#ifndef NEO_EDITORGAME_H
#define NEO_EDITORGAME_H

#include <string>
#include <NeoEngine.h>
#include <Canvas.h>
#include <Menu.h>
#include <TreeView.h>
#include <ScrollPanel.h>
#include <Toolbar.h>
#include <Container.h>
#include <Label.h>
#include <EditField.h>
#include <CheckButton.h>
#include <ImageButton.h>
#include <Project.h>
#include "VectorEdit.h"
#include "NativeToolset.h"
#include "Sidepanel.h"
#include "SceneView.h"
#include "UndoQueue.h"
#include "ColorEdit.h"
#include "Console.h"
#include "BehaviorContainer.h"

#include <sexpresso.hpp>
#include <KeyboardShortcuts.h>

class EditorGame : public Neo::SubGame
{
	Neo2D::Canvas m_canvas;
	shared_ptr<Neo2D::Gui::Menubar> m_menubar;
	shared_ptr<Neo2D::Gui::Toolbar> m_toolbar;
	shared_ptr<Neo2D::Gui::TreeView> m_entityTree;

	shared_ptr<Sidepanel> m_leftPanel;
	shared_ptr<Sidepanel> m_rightPanel;
	shared_ptr<Sidepanel> m_bottomPanel;
	shared_ptr<NativeToolset> m_toolset;

	shared_ptr<SceneView> m_sceneView;

	shared_ptr<Neo2D::Gui::Label> m_diagnosticsLabel;
	shared_ptr<Neo2D::Gui::Container> m_transformUi;
	shared_ptr<Neo2D::Gui::Container> m_entityUi;
	shared_ptr<Neo2D::Gui::Container> m_physicsUi;
	shared_ptr<Neo2D::Gui::Container> m_constraintUi;
	shared_ptr<Neo2D::Gui::Container> m_sceneUi;

	shared_ptr<Neo2D::Gui::Container> m_cameraUi;
	shared_ptr<Neo2D::Gui::Container> m_lightUi;
	shared_ptr<Neo2D::Gui::Container> m_soundUi;
	shared_ptr<Neo2D::Gui::Container> m_textUi;

	shared_ptr<Neo2D::Gui::EditField> m_nameEdit;
	shared_ptr<Vector3Edit> m_positionEdit;
	shared_ptr<Vector3Edit> m_rotationEdit;
	shared_ptr<Vector3Edit> m_scaleEdit;

	shared_ptr<ColorEdit> m_lightColorButton;
	shared_ptr<Vector3Edit> m_lightColorEdit;
	shared_ptr<Neo2D::Gui::EditField> m_lightIntensityEdit;
	shared_ptr<Neo2D::Gui::EditField> m_lightRadiusEdit;
	shared_ptr<Neo2D::Gui::EditField> m_lightSpotAngleEdit;
	shared_ptr<Neo2D::Gui::EditField> m_lightSpotExponentEdit;
	shared_ptr<Neo2D::Gui::EditField> m_lightShadowBiasEdit;
	shared_ptr<Neo2D::Gui::EditField> m_lightShadowBlurEdit;
	shared_ptr<Neo2D::Gui::EditField> m_lightShadowQualityEdit;
	shared_ptr<Neo2D::Gui::CheckButton> m_lightCastShadowButton;

	
	shared_ptr<Neo2D::Gui::EditField> m_cameraFarPlaneEdit;
	shared_ptr<Neo2D::Gui::EditField> m_cameraNearPlaneEdit;
	shared_ptr<Neo2D::Gui::EditField> m_cameraFogDistanceEdit;
	shared_ptr<Neo2D::Gui::EditField> m_cameraFovEdit;
	shared_ptr<ColorEdit> m_cameraFogColorButton;
	shared_ptr<Vector3Edit> m_cameraFogColorEdit;
	shared_ptr<ColorEdit> m_cameraClearColorButton;
	shared_ptr<Vector3Edit> m_cameraClearColorEdit;
	shared_ptr<Neo2D::Gui::CheckButton> m_cameraFogButton;
	shared_ptr<Neo2D::Gui::CheckButton> m_cameraOrthoButton;

	shared_ptr<Neo2D::Gui::EditField> m_soundGainEdit;
	shared_ptr<Neo2D::Gui::EditField> m_soundPitchEdit;
	shared_ptr<Neo2D::Gui::EditField> m_soundRadiusEdit;
	shared_ptr<Neo2D::Gui::EditField> m_soundRolloffEdit;
	shared_ptr<Neo2D::Gui::CheckButton> m_soundLoopingButton;
	shared_ptr<Neo2D::Gui::CheckButton> m_soundRelativeButton;

	shared_ptr<Neo2D::Gui::EditField> m_textTextEdit;
	shared_ptr<Neo2D::Gui::EditField> m_textSizeEdit;
	shared_ptr<ColorEdit> m_textColorButton;
	shared_ptr<Vector4Edit> m_textColorEdit;
		
	shared_ptr<Neo2D::Gui::CheckButton> m_entityInvisibleButton;
	shared_ptr<Neo2D::Gui::EditField> m_entityMassEdit;
	shared_ptr<Neo2D::Gui::EditField> m_entityFrictionEdit;
	shared_ptr<Neo2D::Gui::EditField> m_entityRestitutionEdit;
	shared_ptr<Neo2D::Gui::EditField> m_entityLinearDampingEdit;
	shared_ptr<Neo2D::Gui::EditField> m_entityAngularDampingEdit;
	shared_ptr<Neo2D::Gui::EditField> m_entityAngularFactorEdit;
	shared_ptr<Vector3Edit> m_entityLinearFactorEdit;
	shared_ptr<Neo2D::Gui::Submenu> m_entityShapeMenu;
	shared_ptr<Neo2D::Gui::Button> m_entityMenuButton;

	shared_ptr<Neo2D::Gui::CheckButton> m_entityConstraintButton;
	shared_ptr<Vector3Edit> m_entityPivotEdit;
	shared_ptr<Vector3Edit> m_entityLowerLinearLimitEdit;
	shared_ptr<Vector3Edit> m_entityUpperLinearLimitEdit;
	shared_ptr<Vector3Edit> m_entityLowerAngularLimitEdit;
	shared_ptr<Vector3Edit> m_entityUpperAngularLimitEdit;
	shared_ptr<Neo2D::Gui::CheckButton> m_entityDisableParentCollision;
	shared_ptr<Neo2D::Gui::EditField> m_entityConstraintParentNameEdit;

	shared_ptr<Neo2D::Gui::CheckButton> m_entityPhysicsButton;
	shared_ptr<Neo2D::Gui::CheckButton> m_entityGhostButton;

	shared_ptr<Neo2D::Gui::EditField> m_sceneNameEdit;
	shared_ptr<Neo2D::Gui::EditField> m_sceneScriptFileEdit;
	shared_ptr<Neo2D::Gui::EditField> m_sceneMainCameraEdit;
	shared_ptr<Vector3Edit> m_sceneGravityEdit;
	shared_ptr<ColorEdit> m_sceneAmbientLightEdit;

	shared_ptr<Console> m_console;
	shared_ptr<BehaviorContainer> m_behaviorUi;
	
	std::string m_currentLevelFile;
	std::string m_currentProjectFile;
	Project m_project;

	shared_ptr<Neo2D::Gui::KeyboardShortcuts> m_keyboardShortcuts;

	/// Flag that allows to disable the undo/redo queue
	bool m_disableUndo;
	UndoQueue m_undo;

	std::string* m_visibleObjectsCount;
	
	bool m_isRunningGame;
	weak_ptr<Neo2D::Gui::ImageButton> m_playButton;
	
public:
	EditorGame(const shared_ptr<NativeToolset>& tools) :
		m_toolset(tools),
		m_disableUndo(false),
		m_isRunningGame(false)
	{}

	virtual void update();
	virtual void draw();
	virtual void onBegin();
	virtual void onEnd();

	void updateSceneUi();
	void updateEntityTree();
	void updateSelectedObject(Neo::Object3d* object);
	void updateWindowTitle();

	void runGame();
	void openLevel();
	void openLevel(const char* path);
	void openNewLevel();

	void saveLevel();
	void saveLevel(const char* path);
	void loadProject(const char* path);
	void saveProject(const char* path);

	void addLight();
	void addCamera();
	void addSound();
	void addEntity();
	void addText();

	void deleteSelection();
	void duplicateSelection();

	void undo();
	void redo();

private:
};

#endif //NEO_EDITORGAME_H
