#include "EditorGame.h"
#include "Tool.h"
#include "VectorEdit.h"
#include <Neo2DLevel.h>
#include <ImageButton.h>
#include <Translator.h>
#include <VerticalLayout.h>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <Container.h>
#include <Label.h>
#include <EditField.h>
#include <sstream>

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

	m_sceneView->setPosition(Vector2(m_leftPanel->getSize().x, m_toolbar->getPosition().y - m_toolbar->getSize().y));
	m_sceneView->setSize(Vector2(size.x - m_leftPanel->getSize().x - m_rightPanel->getSize().x, size.y));

	// Update title
	//char title[64];
	//snprintf(title, sizeof(title), "%s (%d FPS)", tr("Neo Scene Editor"), static_cast<int>(floor(1/delta)));
	//engine->getSystemContext()->setWindowTitle(title);

	// Update diagnostics
	std::stringstream ss;
	ss << "Frames Per Second: " << static_cast<int>(floor(1/delta)) << std::endl;
	ss << "Latency: " << delta * 1000.0 << " ms" << std::endl; 
													
	m_diagnosticsLabel->setLabel(ss.str().c_str());

	if(m_sceneView->getSelection().size() && m_sceneView->getState() == Neo2D::Gui::WIDGET_SELECTED)
		updateSelectedObject(m_sceneView->getSelection().back());
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

		m_sceneView->clearSelection();
		engine->getLevel()->clear();
		engine->getLevelLoader()->loadData(filename.c_str(), engine->getLevel());

		engine->getLevel()->getCurrentScene();
		m_sceneView->updateOverlayScene();
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
	m_toolbar = make_shared<Toolbar>(0, m_menubar->getSize().y, 6000, 32, rootpane);

	auto toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/translate.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { m_sceneView->setHandleMode(TRANSLATION); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/scale.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { m_sceneView->setHandleMode(SCALE); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/rotate.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { m_sceneView->setHandleMode(ROTATION); }, nullptr);
	m_toolbar->addWidget(toolbutton);
	
	// Build right panel
	{
			// Right panel
			m_rightPanel = make_shared<Sidepanel>(0, m_menubar->getSize().y +
														 m_toolbar->getSize().y,
												  250, 500, rootpane);
			m_rightPanel->setEdge(RIGHT_EDGE);
			m_rightPanel->setLayout(make_shared<ScaleLayout>());
			m_rightPanel->registerEvent(make_shared<MouseLeftClickEvent>(
				*m_rightPanel, nullptr, nullptr)); // Catch click on panel

			auto rightscroll =
				make_shared<ScrollPanel>(0, 0, 0, 0, m_rightPanel);
			m_rightPanel->addWidget(rightscroll);

			float width = m_rightPanel->getSize().x - 20;

			// Object editing UIs
			m_transformUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 0, rightscroll);
			rightscroll->addWidget(m_transformUi);

			auto scrollLayout = make_shared<VerticalLayout>();
			rightscroll->setLayout(scrollLayout);
			scrollLayout->enableResize(false);
			scrollLayout->setPadding(5);

			m_rightPanel->addWidget(rightscroll);
			rightscroll->addWidget(m_transformUi);

			m_transformUi->setLayout(scrollLayout);

			auto label = make_shared<Label>(0,0,0,10,tr("Name:"), m_transformUi);
			label->setColor(Vector4(0,0,0,1));
			m_transformUi->addWidget(label);
			m_transformUi->addWidget(m_nameEdit = make_shared<EditField>(0,0,width,20,nullptr,m_transformUi));

			label = make_shared<Label>(0,0,0,10,tr("Position:"), m_transformUi);
			label->setColor(Vector4(0,0,0,1));
			m_transformUi->addWidget(label);
			m_transformUi->addWidget(m_positionEdit = make_shared<Vector3Edit>(0,0,width,20,nullptr,m_transformUi));

			label = make_shared<Label>(0,0,0,10,tr("Rotation:"), m_transformUi);
			label->setColor(Vector4(0,0,0,1));
			m_transformUi->addWidget(label);
			m_transformUi->addWidget(m_rotationEdit = make_shared<Vector3Edit>(0,0,width,20,nullptr,m_transformUi));

			label = make_shared<Label>(0,0,0,10,tr("Scale:"), m_transformUi);
			label->setColor(Vector4(0,0,0,1));
			m_transformUi->addWidget(label);
			m_transformUi->addWidget(m_scaleEdit = make_shared<Vector3Edit>(0,0,width,20,nullptr,m_transformUi));

			m_nameEdit->setCallback([this](Widget& w, void* data) {
					MLOG_INFO("Editing name!");
				}, nullptr);

			m_positionEdit->setCallback([this](Widget& w, void* data) {
					if(!m_sceneView->getSelection().size())
						return;
					
					m_sceneView->getSelection().back()->setPosition(m_positionEdit->getVector());
				}, nullptr);
			
			m_rotationEdit->setCallback([this](Widget& w, void* data) {
					if(!m_sceneView->getSelection().size())
						return;
					
					m_sceneView->getSelection().back()->setEulerRotation(m_rotationEdit->getVector());
				}, nullptr);
			
			m_scaleEdit->setCallback([this](Widget& w, void* data) {
					if(!m_sceneView->getSelection().size())
						return;
					
					m_sceneView->getSelection().back()->setScale(m_scaleEdit->getVector());
				}, nullptr);
	}

	// Left panel
	m_leftPanel = make_shared<Sidepanel>(0, m_menubar->getSize().y + m_toolbar->getSize().y, 250, 500, rootpane);
	m_leftPanel->update(0);
	m_leftPanel->setLayout(make_shared<ScaleLayout>());

	auto leftscroll = make_shared<ScrollPanel>(0, m_menubar->getSize().y + m_toolbar->getSize().y, 250, 500, m_leftPanel);
	leftscroll->setLayout(make_shared<VerticalLayout>());
	m_leftPanel->addWidget(leftscroll);
		
	m_entityTree = make_shared<TreeView>(0, m_leftPanel->getPosition().y, 250, 0, leftscroll);

	leftscroll->addWidget(m_entityTree);
	m_leftPanel->addWidget(leftscroll);
	m_leftPanel->setEdge(LEFT_EDGE);

	m_sceneView = make_shared<SceneView>(0,0,0,0, rootpane);
	rootpane->addWidget(m_sceneView);

	m_diagnosticsLabel = make_shared<Label>(m_leftPanel->getSize().x + 20, 10 + m_toolbar->getPosition().y + m_toolbar->getSize().y, 0, 0, "DIAGNOSTICS", rootpane);
	rootpane->addWidget(m_diagnosticsLabel);
	
	rootpane->addWidget(m_leftPanel);
	rootpane->addWidget(m_rightPanel);
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
						updateSelectedObject(m_sceneView->getSelection().back());
					}, nullptr);
		
	NeoEngine* engine = NeoEngine::getInstance();
	engine->getSystemContext()->setWindowTitle(tr("Neo Scene Editor"));
	engine->getGame()->setDrawMainScene(false);
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

void EditorGame::updateSelectedObject(Neo::Object3d* object)
{	
	m_nameEdit->setLabel(object->getName());
	m_nameEdit->setCaret(0);
	
	m_positionEdit->setVector(object->getPosition());
	m_rotationEdit->setVector(object->getEulerRotation());
	m_scaleEdit->setVector(object->getScale());
}
