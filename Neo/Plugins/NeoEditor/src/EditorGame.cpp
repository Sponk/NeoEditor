#include "EditorGame.h"
#include "Tool.h"
#include <Neo2DLevel.h>
#include <Translator.h>
#include <cstdlib>
#include <thread>
#include <atomic>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

// FIXME: Hack!
#ifdef WIN32
#define HOMEDIR getenv("USERPROFILE")
#else
#define HOMEDIR getenv("HOME")
#endif

bool updated = false;
void EditorGame::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	if(!updated)
	{
		updateEntityTree();
		updated = true;
	}

	float delta = engine->getGame()->getFrameDelta();

	PROFILE_BEGIN("GuiUpdate");
	m_canvas.update(engine->getGame()->getFrameDelta());
	PROFILE_END("GuiUpdate");

	Vector2 const size = engine->getSystemContext()->getScreenSize();

	m_sceneView->update(delta);

	m_sceneView->setPosition(Vector2(m_leftPanel->getSize().x, m_toolbar->getPosition().y - m_toolbar->getSize().y));
	m_sceneView->setSize(Vector2(size.x - m_leftPanel->getSize().x, size.y));
}

void EditorGame::draw()
{
	PROFILE_BEGIN("GuiDraw");
	m_canvas.draw();
	PROFILE_END("GuiDraw");
}

void EditorGame::onBegin()
{
	// TODO: Load config!
	auto rootpane = make_shared<Container>(0, 0, 0, 0, nullptr);
	m_canvas.addObject2D(rootpane);

	m_menubar = make_shared<Menubar>(0, 0, 6000, 25, rootpane);
	auto filemenu = make_shared<Submenu>(tr("File"), m_menubar);

	filemenu->addItem(tr("New Level"), nullptr);
	filemenu->addItem(tr("Open Project"), [this](Widget&, void*) {
		std::string filename = m_toolset->fileOpenDialog("Open Project", HOMEDIR, "Projects (*.neo)");
		MLOG_INFO("FILENAME: " << filename);
	});

	filemenu->addItem(tr("Open Level"), [this](Widget&, void*) {
		std::string filename = m_toolset->fileOpenDialog("Open Level", HOMEDIR, "Levels (*.level)");

		/*std::atomic<bool> done(false);
		std::thread up([filename, this, &done]()
					{
						Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
						Level* level = new Level;
						//engine->getLevel()->clear();
						engine->getLevelLoader()->loadData(filename.c_str(), level);

						// Secure old level and put in the new one
						Level* oldlevel = engine->getLevel();
						engine->setLevel(level);

						SAFE_DELETE(oldlevel);
						updated = false;
						done = true;
					});

		Neo::NeoGame* game = Neo::NeoEngine::getInstance()->getGame();
		while(!done)
		{
			game->update();
			game->draw();
		}

		up.join();*/

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		engine->getLevel()->clear();
		engine->getLevelLoader()->loadData(filename.c_str(), engine->getLevel());

		engine->getLevel()->getCurrentScene();
		updated = false;
	});

	filemenu->addItem(tr("Save"), nullptr);
	filemenu->addItem(tr("Save as..."), [this](Widget&, void*) {
		std::string filename = m_toolset->fileSaveDialog("Save Level as...", HOMEDIR, "Images (*.jpg *.png *.tiff)");
		MLOG_INFO("FILENAME: " << filename);
	});

	filemenu->addItem(tr("Quit"), [] (Widget&, void*) { NeoEngine::getInstance()->setActive(false); });
	m_menubar->addMenu(filemenu);

	// Toolbar
	m_toolbar = make_shared<Toolbar>(0, m_menubar->getSize().y, 6000, 25, rootpane);

	// Sidepanels
	m_leftPanel = make_shared<Sidepanel>(0, m_menubar->getSize().y + m_toolbar->getSize().y, 250, 500, rootpane);
	m_leftPanel->setLayout(make_shared<ScaleLayout>());

	auto leftscroll = make_shared<ScrollPanel>(0, m_menubar->getSize().y + m_toolbar->getSize().y, 250, 500, m_leftPanel);
	m_entityTree = make_shared<TreeView>(0, m_leftPanel->getPosition().y, 250, 0, leftscroll);

	leftscroll->addWidget(m_entityTree);
	m_leftPanel->addWidget(leftscroll);
	m_leftPanel->setEdge(LEFT_EDGE);

	m_sceneView = make_shared<SceneView>(0,0,0,0, rootpane);
	rootpane->addWidget(m_sceneView);

	rootpane->addWidget(m_leftPanel);
	rootpane->addWidget(m_toolbar);
	rootpane->addWidget(m_menubar);

	m_entityTree->setCallback([this](Widget& w, void*) {
		auto tree = static_cast<TreeView&>(w);
		const char* selected = tree.getSelected();
		NeoEngine* engine = NeoEngine::getInstance();

		if(!selected) return;
		Object3d* obj = engine->getLevel()->getCurrentScene()->getObjectByName(selected);

		if(obj)
		{
			if(!engine->getInputContext()->isKeyDown(KEY_LSHIFT))
				m_sceneView->clearSelection();

			m_sceneView->addSelectedObject(obj);
		}

	}, nullptr);

	m_sceneView->setCallback([this](Widget& w, void*) {
						m_entityTree->setSelected(m_sceneView->getSelection().back()->getName());
					}, nullptr);

	NeoEngine::getInstance()->getGame()->setDrawMainScene(false);
}

void EditorGame::onEnd()
{
	Neo2D::Neo2DLevel::getInstance()->clear();
}

void EditorGame::updateEntityTree()
{
	NeoEngine* engine = NeoEngine::getInstance();
	auto scene = engine->getLevel()->getCurrentScene();

	std::function<void(TreeView::TreeNode*, Object3d*)> updateChildren = [&updateChildren](TreeView::TreeNode* node, Object3d* object)
	{
		for(int i = 0; i < object->getChildrenNumber(); i++)
		{
			updateChildren(node->addChild(object->getChild(i)->getName()).get(), object->getChild(i));
		}
	};

	m_entityTree->getRoot()->clear();
	for(int i = 0; i < scene->getObjectsNumber(); i++)
	{
		updateChildren(m_entityTree->getRoot()->addChild(scene->getObjectByIndex(i)->getName()).get(), scene->getObjectByIndex(i));
	}

	m_sceneView->updateOverlayScene();
}
