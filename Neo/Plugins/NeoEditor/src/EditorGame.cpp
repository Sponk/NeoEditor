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

#define MAKE_LABEL(str, ui)                                        \
	{                                                              \
		auto label = make_shared<Label>(0, 0, 0, 10, tr(str), ui); \
		label->setColor(Vector4(0, 0, 0, 1));                      \
		ui->addWidget(label);                                      \
	}

#define MAKE_FLOAT_EDIT_FIELD(str, width, edit, ui, type, setter)               \
	{                                                                     \
		MAKE_LABEL(str, ui);                                              \
		ui->addWidget(                                                    \
			edit = make_shared<EditField>(0, 0, width, 20, nullptr, ui)); \
		edit->setCallback(                                                \
			[this](Widget& w, void* d) {                                  \
				if (!m_sceneView->getSelection().size())                  \
					return;                                               \
                                                                          \
				m_undo.save();                                            \
				static_cast<type*>(m_sceneView->getSelection().back())    \
					->setter(std::stod(edit->getLabel()));                \
			},                                                            \
			nullptr);                                                     \
	}

#define MAKE_STRING_EDIT_FIELD(str, width, edit, ui, type, setter)               \
	{                                                                     \
		MAKE_LABEL(str, ui);                                              \
		ui->addWidget(                                                    \
			edit = make_shared<EditField>(0, 0, width, 20, nullptr, ui)); \
		edit->setCallback(                                                \
			[this](Widget& w, void* d) {                                  \
				if (!m_sceneView->getSelection().size())                  \
					return;                                               \
                                                                          \
				m_undo.save();                                            \
				static_cast<type*>(m_sceneView->getSelection().back())    \
					->setter(edit->getLabel());							\
			},                                                            \
			nullptr);                                                     \
	}

#define MAKE_3D_EDIT_FIELD(str, width, edit, ui, type, setter)               \
	{                                                                     \
		MAKE_LABEL(str, ui);                                              \
		ui->addWidget(                                                    \
			edit = make_shared<Vector3Edit>(0, 0, width, 20, nullptr, ui)); \
		edit->setCallback(                                                \
			[this](Widget& w, void* d) {                                  \
				if (!m_sceneView->getSelection().size())                  \
					return;                                               \
                                                                          \
				m_undo.save();                                            \
				static_cast<type*>(m_sceneView->getSelection().back())    \
					->setter(edit->getVector());				\
			},                                                            \
			nullptr);                                                     \
	}

#define MAKE_4D_EDIT_FIELD(str, width, edit, ui, type, setter)               \
	{                                                                     \
		MAKE_LABEL(str, ui);                                              \
		ui->addWidget(                                                    \
			edit = make_shared<Vector4Edit>(0, 0, width, 20, nullptr, ui)); \
		edit->setCallback(                                                \
			[this](Widget& w, void* d) {                                  \
				if (!m_sceneView->getSelection().size())                  \
					return;                                               \
                                                                          \
				m_undo.save();                                            \
				static_cast<type*>(m_sceneView->getSelection().back())    \
					->setter(edit->getVector());				\
			},                                                            \
			nullptr);                                                     \
	}

#define MAKE_CHECK_BUTTON(str, edit, ui, type, setter)                    \
	{ \
			ui->addWidget(                                                \
			edit = make_shared<CheckButton>(0, 0, 100, 20, tr(str), ui)); \
		edit->setCallback(                                                \
			[this](Widget& w, void* d) {                                  \
				if (!m_sceneView->getSelection().size())                  \
					return;                                               \
                                                                          \
				m_undo.save();                                            \
				static_cast<type*>(m_sceneView->getSelection().back())    \
					->setter(edit->getValue());                           \
			},                                                            \
			nullptr);                                                     \
	}

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
	m_canvas.update(delta);
	PROFILE_END("GuiUpdate");

	Vector2 const size = engine->getSystemContext()->getScreenSize();

	m_sceneView->setPosition(Vector2(m_leftPanel->getSize().x, m_toolbar->getPosition().y - m_toolbar->getSize().y));
	m_sceneView->setSize(Vector2(size.x - m_leftPanel->getSize().x - m_rightPanel->getSize().x - 15, size.y));

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
	// Toolbar
	m_toolbar = make_shared<Toolbar>(0, m_menubar->getSize().y, 6000, 32, rootpane);

	
	auto toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/document-open.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { openLevel(); }, nullptr);
	m_toolbar->addWidget(toolbutton);
	
	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/document-save.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { saveLevel(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/edit-undo.png", m_toolbar);
	toolbutton->setCallback(
		[this](Widget&, void*) {
			m_sceneView->clearSelection();
			m_undo.undo();
			//m_sceneView->updateOverlayScene();
			updated = false;
		},
		nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0, 0, 32, 32, "data/icons/edit-redo.png", m_toolbar);
	toolbutton->setCallback(
		[this](Widget&, void*) {
			m_sceneView->clearSelection();
			m_undo.redo();
			//m_sceneView->updateOverlayScene();
			updated = false;
		},
		nullptr);
	
	m_toolbar->addWidget(toolbutton);
		
	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/translate.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { m_sceneView->setHandleMode(TRANSLATION); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/scale.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { m_sceneView->setHandleMode(SCALE); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/rotate.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { m_sceneView->setHandleMode(ROTATION); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	auto filemenu = make_shared<Submenu>(tr("File"), m_menubar);
	auto editmenu = make_shared<Submenu>(tr("Edit"), m_menubar);
	auto helpmenu = make_shared<Submenu>(tr("Help"), m_menubar);
	
	filemenu->addItem(tr("New Level"), [this](Widget&, void*) {
			
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		std::string path =  m_toolset->fileSaveDialog(tr("Save Level"), HOMEDIR, "Levels (*.level)");

		if(path.empty())
			return;

		m_currentLevelFile = path;
		m_sceneView->clearSelection();
		engine->getLevel()->clear();

		Level empty;
		auto mainscene = empty.addNewScene();
		mainscene->setName("Scene-1");
			
		engine->getLevelLoader()->saveData(m_currentLevelFile.c_str(), "level", &empty);
		engine->getLevelLoader()->loadData(m_currentLevelFile.c_str(), engine->getLevel());
		
		engine->getLevel()->getCurrentScene();
		m_sceneView->updateOverlayScene();
			
		updated = false;
		updateWindowTitle();
	});
	
	filemenu->addItem(tr("Open Project"), [this](Widget&, void*) {
		std::string filename = m_toolset->fileOpenDialog("Open Project", HOMEDIR, "Projects (*.neo)");
	});

	filemenu->addItem(tr("Open Level"), [this](Widget&, void*) {
			openLevel();
	});

	filemenu->addItem(tr("Save"), [this](Widget&, void*) {
			saveLevel();
	});

	filemenu->addItem(tr("Save as..."), [this](Widget&, void*) {
			std::string curfile = m_currentLevelFile;
			m_currentLevelFile.clear();

			saveLevel();

			// If no level was chosen, reset to old value
			if(m_currentLevelFile.empty())
				m_currentLevelFile = curfile;
	});

	filemenu->addItem(tr("Quit"), [] (Widget&, void*) { NeoEngine::getInstance()->setActive(false); });

	helpmenu->addItem(tr("About"), [this](Widget&, void*) { m_toolset->aboutDialog(); });
	
	m_menubar->addMenu(filemenu);
	m_menubar->addMenu(editmenu);
	m_menubar->addMenu(helpmenu);
		
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
												   width, 160, rightscroll);

			m_entityUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 200, rightscroll);

			m_lightUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 200, rightscroll);
			
			m_cameraUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 200, rightscroll);

			m_soundUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 200, rightscroll);

			m_textUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 200, rightscroll);
			
			rightscroll->addWidget(m_transformUi);
			rightscroll->addWidget(m_entityUi);
			rightscroll->addWidget(m_lightUi);
			rightscroll->addWidget(m_cameraUi);
			rightscroll->addWidget(m_soundUi);
			rightscroll->addWidget(m_textUi);
			
			auto scrollLayout = make_shared<VerticalLayout>();
			rightscroll->setLayout(scrollLayout);
			scrollLayout->enableResize(false);
			scrollLayout->setPadding(5);

			m_rightPanel->addWidget(rightscroll);
			m_transformUi->setLayout(scrollLayout);
			m_entityUi->setLayout(scrollLayout);
			m_lightUi->setLayout(scrollLayout);
			m_cameraUi->setLayout(scrollLayout);
			m_soundUi->setLayout(scrollLayout);
			m_textUi->setLayout(scrollLayout);
			
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

				if(!m_sceneView->getSelection().size())
					return;

				const char* name = m_nameEdit->getLabel();
				if(strlen(name) == 0)
				{
					m_toolset->messagebox(tr("Error"), tr("Please enter a name that is not empty!"));
					return;
				}

				if(NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(name))
				{
					m_toolset->messagebox(tr("Error"), tr("An object with that name already exists. Please choose another one."));
					return;
				}

				m_sceneView->getSelection().back()->setName(name);
				m_entityTree->findNode(m_entityTree->getSelected())->setLabel(name);
				m_undo.save();

			}, nullptr);

			m_positionEdit->setCallback([this](Widget& w, void* data) {
					if(!m_sceneView->getSelection().size())
						return;

					m_undo.save();
					m_sceneView->getSelection().back()->setPosition(m_positionEdit->getVector());
				}, nullptr);
			
			m_rotationEdit->setCallback([this](Widget& w, void* data) {
					if(!m_sceneView->getSelection().size())
						return;

					m_undo.save();
					m_sceneView->getSelection().back()->setEulerRotation(m_rotationEdit->getVector());
				}, nullptr);
			
			m_scaleEdit->setCallback([this](Widget& w, void* data) {
					if(!m_sceneView->getSelection().size())
						return;

					m_undo.save();
					m_sceneView->getSelection().back()->setScale(m_scaleEdit->getVector());
				}, nullptr);

			MAKE_CHECK_BUTTON("Invisible", m_entityInvisibleButton, m_entityUi, OEntity, setInvisible);
			
			// Hide UI initially
			m_entityUi->setActive(false);
			m_entityUi->setInvisible(true);

			m_lightUi->setActive(false);
			m_lightUi->setInvisible(true);

			m_cameraUi->setActive(false);
			m_cameraUi->setInvisible(true);

			m_soundUi->setActive(false);
			m_soundUi->setInvisible(true);

			m_textUi->setActive(false);
			m_textUi->setInvisible(true);

			// Light UI
			MAKE_3D_EDIT_FIELD("Color:", width, m_lightColorEdit, m_lightUi, OLight, setColor);
			MAKE_FLOAT_EDIT_FIELD("Intensity:", width, m_lightIntensityEdit, m_lightUi, OLight, setIntensity);
			MAKE_FLOAT_EDIT_FIELD("Radius:", width, m_lightRadiusEdit, m_lightUi, OLight, setRadius);
			MAKE_FLOAT_EDIT_FIELD("Spot Angle:", width, m_lightSpotAngleEdit, m_lightUi, OLight, setSpotAngle);
			MAKE_FLOAT_EDIT_FIELD("Spot Exponent:", width, m_lightSpotExponentEdit, m_lightUi, OLight, setSpotExponent);
			MAKE_CHECK_BUTTON("Cast Shadows", m_lightCastShadowButton, m_lightUi, OLight, castShadow);
			MAKE_FLOAT_EDIT_FIELD("Shadow Quality:", width, m_lightShadowQualityEdit, m_lightUi, OLight, setShadowQuality);
			MAKE_FLOAT_EDIT_FIELD("Shadow Bias:", width, m_lightShadowBiasEdit, m_lightUi, OLight, setShadowBias);
			MAKE_FLOAT_EDIT_FIELD("Shadow Blur:", width, m_lightShadowBlurEdit, m_lightUi, OLight, setShadowBlur);

			// Camera UI
			MAKE_FLOAT_EDIT_FIELD("Near Plane:", width, m_cameraNearPlaneEdit, m_cameraUi, OCamera, setClippingNear);
			MAKE_FLOAT_EDIT_FIELD("Far Plane:", width, m_cameraFarPlaneEdit, m_cameraUi, OCamera, setClippingFar);
			MAKE_FLOAT_EDIT_FIELD("Field of View:", width, m_cameraFovEdit, m_cameraUi, OCamera, setFov);
			MAKE_CHECK_BUTTON("Ortho", m_cameraOrthoButton, m_cameraUi, OCamera, enableOrtho);
			MAKE_CHECK_BUTTON("Fog", m_cameraFogButton, m_cameraUi, OCamera, enableFog);
			MAKE_FLOAT_EDIT_FIELD("Fog Distance:", width, m_cameraFogDistanceEdit, m_cameraUi, OCamera, setFogDistance);
			MAKE_3D_EDIT_FIELD("Fog Color:", width, m_cameraFogColorEdit, m_cameraUi, OCamera, setFogColor);
			MAKE_3D_EDIT_FIELD("Clear Color:", width, m_cameraClearColorEdit, m_cameraUi, OCamera, setClearColor);

			// Sound UI
			MAKE_FLOAT_EDIT_FIELD("Gain:", width, m_soundGainEdit, m_soundUi, OSound, setGain);
			MAKE_FLOAT_EDIT_FIELD("Pitch:", width, m_soundPitchEdit, m_soundUi, OSound, setPitch);
			MAKE_FLOAT_EDIT_FIELD("Radius:", width, m_soundRadiusEdit, m_soundUi, OSound, setRadius);
			MAKE_FLOAT_EDIT_FIELD("Rolloff:", width, m_soundRolloffEdit, m_soundUi, OSound, setRolloff);
			MAKE_CHECK_BUTTON("Looping", m_soundLoopingButton, m_cameraUi, OSound, setLooping);
			MAKE_CHECK_BUTTON("Relative", m_soundRelativeButton, m_cameraUi, OSound, setRelative);			

			// Text UI
			MAKE_STRING_EDIT_FIELD("Text:", width, m_textTextEdit, m_textUi, OText, setText);
			MAKE_FLOAT_EDIT_FIELD("Size:", width, m_textSizeEdit, m_textUi, OText, setSize);
			MAKE_4D_EDIT_FIELD("Color:", width, m_textColorEdit, m_textUi, OText, setColor);
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

	m_sceneView = make_shared<SceneView>(m_undo, 0,0,0,0, rootpane);
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
			updateSelectedObject(obj);
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

	m_entityUi->setActive(false);
	m_entityUi->setInvisible(true);

	m_lightUi->setActive(false);
	m_lightUi->setInvisible(true);

	m_cameraUi->setActive(false);
	m_cameraUi->setInvisible(true);

	m_soundUi->setActive(false);
	m_soundUi->setInvisible(true);
	
	m_textUi->setActive(false);
	m_textUi->setInvisible(true);
	
	switch(object->getType())
	{
	case OBJECT3D_ENTITY: {
		m_entityUi->setActive(true);
		m_entityUi->setInvisible(false);

		OEntity* entity = static_cast<OEntity*>(object);
		m_entityInvisibleButton->setValue(entity->isInvisible());
	}
	break;

	case OBJECT3D_LIGHT: {
		m_lightUi->setActive(true);
		m_lightUi->setInvisible(false);

		OLight* light = static_cast<OLight*>(object);
		m_lightColorEdit->setVector(light->getColor());
		m_lightIntensityEdit->setLabel(std::to_string(light->getIntensity()).c_str());
		m_lightRadiusEdit->setLabel(std::to_string(light->getRadius()).c_str());
		m_lightSpotAngleEdit->setLabel(std::to_string(light->getSpotAngle()).c_str());
		m_lightSpotExponentEdit->setLabel(std::to_string(light->getSpotExponent()).c_str());
		m_lightShadowQualityEdit->setLabel(std::to_string(light->getShadowQuality()).c_str());
		m_lightShadowBiasEdit->setLabel(std::to_string(light->getShadowBias()).c_str());
		m_lightShadowBlurEdit->setLabel(std::to_string(light->getShadowBlur()).c_str());
		m_lightCastShadowButton->setValue(light->isCastingShadow());
	}
	break;

	case OBJECT3D_CAMERA: {
		m_cameraUi->setActive(true);
		m_cameraUi->setInvisible(false);

		OCamera* cam = static_cast<OCamera*>(object);
		m_cameraFarPlaneEdit->setLabel(std::to_string(cam->getClippingFar()).c_str());
		m_cameraNearPlaneEdit->setLabel(std::to_string(cam->getClippingNear()).c_str());
		m_cameraFovEdit->setLabel(std::to_string(cam->getFov()).c_str());
		m_cameraFogButton->setValue(cam->hasFog());
		m_cameraOrthoButton->setValue(cam->isOrtho());
		m_cameraClearColorEdit->setVector(cam->getClearColor());
		m_cameraFogColorEdit->setVector(cam->getFogColor());
		m_cameraFogDistanceEdit->setLabel(std::to_string(cam->getFogDistance()).c_str());
		
	}
	break;
	
	case OBJECT3D_SOUND: {
		m_soundUi->setActive(true);
		m_soundUi->setInvisible(false);

		OSound* sound = static_cast<OSound*>(object);
		m_soundGainEdit->setLabel(std::to_string(sound->getGain()).c_str());
		m_soundPitchEdit->setLabel(std::to_string(sound->getPitch()).c_str());
		m_soundRolloffEdit->setLabel(std::to_string(sound->getRolloff()).c_str());
		m_soundRadiusEdit->setLabel(std::to_string(sound->getRadius()).c_str());
		m_soundLoopingButton->setValue(sound->isLooping());
		m_soundRelativeButton->setValue(sound->isRelative());
	}
	break;

	case OBJECT3D_TEXT: {
		m_textUi->setActive(true);
		m_textUi->setInvisible(false);

		OText* text = static_cast<OText*>(object);
		m_textTextEdit->setLabel(text->getText());
		m_textColorEdit->setVector(text->getColor());
		m_textSizeEdit->setLabel(std::to_string(text->getSize()).c_str());
	}
	break;
	}
}

void EditorGame::openLevel()
{
		std::string filename = m_toolset->fileOpenDialog("Open Level", HOMEDIR, "Levels (*.level)");
		if(filename.empty())
			return;
		
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

		m_sceneView->updateOverlayScene();
		updated = false;

		m_undo.clear();
		m_undo.save();

		m_currentLevelFile = filename;
		updateWindowTitle();
}

void EditorGame::saveLevel()
{	
	if(m_currentLevelFile.empty())
		m_currentLevelFile = m_toolset->fileSaveDialog("Save Level", HOMEDIR, "Levels (*.level)");

	if(m_currentLevelFile.empty())
		return;

	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getLevelLoader()->saveData(m_currentLevelFile.c_str(), "level", engine->getLevel());
	updateWindowTitle();
}

void EditorGame::saveLevel(const char* path)
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	engine->getLevelLoader()->saveData(path, "level", engine->getLevel());
}

void EditorGame::updateWindowTitle()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	std::stringstream ss;
	ss << tr("Neo Scene Editor") << " - " << m_currentLevelFile;
	engine->getSystemContext()->setWindowTitle(ss.str().c_str());
}
