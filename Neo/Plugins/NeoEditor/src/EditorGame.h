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
#include <Project.h>
#include "VectorEdit.h"
#include "NativeToolset.h"
#include "Sidepanel.h"
#include "SceneView.h"
#include "UndoQueue.h"
#include "ColorEdit.h"

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
	shared_ptr<NativeToolset> m_toolset;

	shared_ptr<SceneView> m_sceneView;

	shared_ptr<Neo2D::Gui::Label> m_diagnosticsLabel;
	shared_ptr<Neo2D::Gui::Container> m_transformUi;
	shared_ptr<Neo2D::Gui::Container> m_entityUi;
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
	std::string m_currentLevelFile;
	std::string m_currentProjectFile;
	Project m_project;

	shared_ptr<Neo2D::Gui::KeyboardShortcuts> m_keyboardShortcuts;

	UndoQueue m_undo;
	
public:
	EditorGame(const shared_ptr<NativeToolset>& tools) :
		m_toolset(tools)
	{}

	virtual void update();
	virtual void draw();
	virtual void onBegin();
	virtual void onEnd();

	void updateEntityTree();
	void updateSelectedObject(Neo::Object3d* object);
	void updateWindowTitle();
	
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
