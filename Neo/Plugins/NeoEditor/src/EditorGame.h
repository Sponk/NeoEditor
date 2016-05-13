#ifndef NEO_EDITORGAME_H
#define NEO_EDITORGAME_H

#include <NeoEngine.h>
#include <Canvas.h>
#include <Menu.h>
#include <TreeView.h>
#include <ScrollPanel.h>
#include <Toolbar.h>
#include "NativeToolset.h"
#include "Sidepanel.h"

class EditorGame : public Neo::SubGame
{
	Neo2D::Canvas m_canvas;
	shared_ptr<Neo2D::Gui::Menubar> m_menubar;
	shared_ptr<Neo2D::Gui::Toolbar> m_toolbar;
	shared_ptr<Neo2D::Gui::TreeView> m_entityTree;

	shared_ptr<Sidepanel> m_leftPanel;

	shared_ptr<NativeToolset> m_toolset;

public:
	EditorGame(const shared_ptr<NativeToolset>& tools) : m_toolset(tools) {}

	virtual void update();
	virtual void draw();
	virtual void onBegin();
	virtual void onEnd();

	void updateEntityTree();

private:
};

#endif //NEO_EDITORGAME_H
