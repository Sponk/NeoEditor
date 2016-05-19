#ifndef NEO_EDITORGAME_H
#define NEO_EDITORGAME_H

#include <NeoEngine.h>
#include <Canvas.h>
#include <Menu.h>
#include <TreeView.h>
#include <ScrollPanel.h>
#include <Toolbar.h>
#include <Container.h>
#include <Label.h>
#include <EditField.h>
#include "VectorEdit.h"
#include "NativeToolset.h"
#include "Sidepanel.h"
#include "SceneView.h"

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
	shared_ptr<Neo2D::Gui::Container> m_lightUi;
	shared_ptr<Neo2D::Gui::Container> m_soundUi;
	shared_ptr<Neo2D::Gui::Container> m_textUi;

	
	shared_ptr<Neo2D::Gui::EditField> m_nameEdit;
	shared_ptr<Vector3Edit> m_positionEdit;
	shared_ptr<Vector3Edit> m_rotationEdit;
	shared_ptr<Vector3Edit> m_scaleEdit;
	
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
	
private:
};

#endif //NEO_EDITORGAME_H
