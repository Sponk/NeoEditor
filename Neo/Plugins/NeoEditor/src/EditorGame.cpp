#include "EditorGame.h"
#include <Neo2DLevel.h>
#include <ImageButton.h>
#include <Translator.h>
#include <VerticalLayout.h>
#include <thread>
#include <atomic>
#include <Player.h>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

// FIXME: Hack!
#ifdef WIN32
#define HOMEDIR getenv("USERPROFILE")

#include <direct.h> // _getcwd
#define getcwd _getcwd

#else
#define HOMEDIR getenv("HOME")

#include <unistd.h> // getcwd

#endif

#define MAKE_LABEL(str, ui)                                        \
	{                                                              \
		auto label = make_shared<Label>(0, 0, 0, 10, str, ui); \
		label->setColor(Vector4(0, 0, 0, 1));                      \
		ui->addWidget(label);                                      \
	}

#define MAKE_FLOAT_EDIT_FIELD(str, width, edit, ui, type, setter)         \
	{                                                                     \
		MAKE_LABEL(str, ui);                                              \
		ui->addWidget(                                                    \
			edit = make_shared<EditField>(0, 0, width, 20, nullptr, ui)); \
		edit->setDebugString(str); \
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

#define MAKE_FLOAT_PHYSICS_EDIT_FIELD(str, width, edit, ui, setter)            \
	{                                                                          \
		MAKE_LABEL(str, ui);                                                   \
		ui->addWidget(                                                         \
			edit = make_shared<EditField>(0, 0, width, 20, nullptr, ui));      \
		edit->setDebugString(str); \
		edit->setCallback(                                                     \
			[this](Widget& w, void* d) {                                       \
				if (!m_sceneView->getSelection().size())                       \
					return;                                                    \
                                                                               \
				m_undo.save();                                                 \
				auto entity =                                                  \
					static_cast<OEntity*>(m_sceneView->getSelection().back()); \
				auto phys = entity->getPhysicsProperties();                    \
				if (!phys)                                                     \
					return;                                                    \
                                                                               \
				phys->setter(std::stod(edit->getLabel()));                     \
			},                                                                 \
			nullptr);                                                          \
	}

#define MAKE_STRING_EDIT_FIELD(str, width, edit, ui, type, setter)        \
	{                                                                     \
		MAKE_LABEL(str, ui);                                              \
		ui->addWidget(                                                    \
			edit = make_shared<EditField>(0, 0, width, 20, nullptr, ui)); \
		edit->setDebugString(str); \
		edit->setCallback(                                                \
			[this](Widget& w, void* d) {                                  \
				if (!m_sceneView->getSelection().size())                  \
					return;                                               \
                                                                          \
				m_undo.save();                                            \
				static_cast<type*>(m_sceneView->getSelection().back())    \
					->setter(edit->getLabel());                           \
			},                                                            \
			nullptr);                                                     \
	}

#define MAKE_SCENE_STRING_EDIT_FIELD(str, width, edit, ui, setter)        \
	{                                                                     \
		MAKE_LABEL(str, ui);                                              \
		ui->addWidget(                                                    \
			edit = make_shared<EditField>(0, 0, width, 20, nullptr, ui)); \
		edit->setCallback(                                                \
			[this](Widget& w, void* d) {                                  \
				                                                          \
				m_undo.save();                                            \
				NeoEngine::getInstance()->getLevel()->getCurrentScene()   \
					->setter(edit->getLabel());                           \
			},                                                            \
			nullptr);                                                     \
	}

#define MAKE_CONSTRAINT_STRING_EDIT_FIELD(str, width, edit, ui, setter)       \
	{                                                                         \
		MAKE_LABEL(str, ui);                                                  \
		ui->addWidget(                                                        \
			edit = make_shared<EditField>(0, 0, width, 20, nullptr, ui));     \
		edit->setDebugString(str); \
		edit->setCallback(                                                    \
			[this](Widget& w, void* d) {                                      \
				if (!m_sceneView->getSelection().size())                      \
					return;                                                   \
                                                                              \
				m_undo.save();                                                \
				auto phys =                                                   \
					static_cast<OEntity*>(m_sceneView->getSelection().back()) \
						->getPhysicsProperties();                             \
				if (!phys)                                                    \
					return;                                                   \
                                                                              \
				auto con = phys->getConstraint();                             \
				if (!con)                                                     \
					return;                                                   \
                                                                              \
				con->setter.set(edit->getLabel());                            \
			},                                                                \
			nullptr);                                                         \
	}

#define MAKE_3D_EDIT_FIELD(str, width, edit, ui, type, setter)              \
	{                                                                       \
		MAKE_LABEL(str, ui);                                                \
		ui->addWidget(                                                      \
			edit = make_shared<Vector3Edit>(0, 0, width, 20, nullptr, ui)); \
		edit->setDebugString(str); \
		edit->setCallback(                                                  \
			[this](Widget& w, void* d) {                                    \
				if (!m_sceneView->getSelection().size())                    \
					return;                                                 \
                                                                            \
				m_undo.save();                                              \
				static_cast<type*>(m_sceneView->getSelection().back())      \
					->setter(edit->getVector());                            \
				updateSelectedObject(m_sceneView->getSelection().back());   \
			},                                                              \
			nullptr);                                                       \
	}

#define MAKE_3D_SCENE_EDIT_FIELD(str, width, edit, ui, setter)              \
	{                                                                       \
		MAKE_LABEL(str, ui);                                                \
		ui->addWidget(                                                      \
			edit = make_shared<Vector3Edit>(0, 0, width, 20, nullptr, ui)); \
		edit->setDebugString(str); \
		edit->setCallback(                                                  \
			[this](Widget& w, void* d) {                                    \
				if (!m_sceneView->getSelection().size())                    \
					return;                                                 \
                                                                            \
				m_undo.save();                                            	\
				NeoEngine::getInstance()->getLevel()->getCurrentScene()   	\
					->setter(edit->getVector());                           	\
			},                                                              \
			nullptr);                                                       \
	}

#define MAKE_3D_PHYSICS_EDIT_FIELD(str, width, edit, ui, setter)              \
	{                                                                         \
		MAKE_LABEL(str, ui);                                                  \
		ui->addWidget(                                                        \
			edit = make_shared<Vector3Edit>(0, 0, width, 20, nullptr, ui));   \
		edit->setDebugString(str); \
		edit->setCallback(                                                    \
			[this](Widget& w, void* d) {                                      \
				if (!m_sceneView->getSelection().size())                      \
					return;                                                   \
                                                                              \
				m_undo.save();                                                \
                                                                              \
				auto phys =                                                   \
					static_cast<OEntity*>(m_sceneView->getSelection().back()) \
						->getPhysicsProperties();                             \
				if (!phys)                                                    \
					return;                                                   \
                                                                              \
				phys->setter(edit->getVector());                              \
				updateSelectedObject(m_sceneView->getSelection().back());     \
			},                                                                \
			nullptr);                                                         \
	}

#define MAKE_3D_CONSTRAINT_EDIT_FIELD(str, width, edit, ui, setter)           \
	{                                                                         \
		MAKE_LABEL(str, ui);                                                  \
		ui->addWidget(                                                        \
			edit = make_shared<Vector3Edit>(0, 0, width, 20, nullptr, ui));   \
		edit->setDebugString(str); \
		edit->setCallback(                                                    \
			[this](Widget& w, void* d) {                                      \
				if (!m_sceneView->getSelection().size())                      \
					return;                                                   \
                                                                              \
				m_undo.save();                                                \
                                                                              \
				auto phys =                                                   \
					static_cast<OEntity*>(m_sceneView->getSelection().back()) \
						->getPhysicsProperties();                             \
				if (!phys)                                                    \
					return;                                                   \
                                                                              \
				auto con = phys->getConstraint();                             \
				if (!con)                                                     \
					return;                                                   \
                                                                              \
				con->setter = edit->getVector();                              \
				updateSelectedObject(m_sceneView->getSelection().back());     \
			},                                                                \
			nullptr);                                                         \
	}

#define MAKE_4D_EDIT_FIELD(str, width, edit, ui, type, setter)              \
	{                                                                       \
		MAKE_LABEL(str, ui);                                                \
		ui->addWidget(                                                      \
			edit = make_shared<Vector4Edit>(0, 0, width, 20, nullptr, ui)); \
		edit->setDebugString(str); \
		edit->setCallback(                                                  \
			[this](Widget& w, void* d) {                                    \
				if (!m_sceneView->getSelection().size())                    \
					return;                                                 \
                                                                            \
				m_undo.save();                                              \
				static_cast<type*>(m_sceneView->getSelection().back())      \
					->setter(edit->getVector());                            \
				updateSelectedObject(m_sceneView->getSelection().back());   \
			},                                                              \
			nullptr);                                                       \
	}

#define MAKE_CHECK_BUTTON(str, edit, ui, type, setter)                    \
	{                                                                     \
		ui->addWidget(                                                    \
			edit = make_shared<CheckButton>(0, 0, 100, 20, str, ui)); \
		edit->setDebugString(str); \
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

#define MAKE_BUTTON(str, width, ui, callback)                         \
	{                                                                 \
		auto btn = make_shared<Button>(0, 0, width, 20, str, ui); \
		ui->addWidget(btn);                                           \
		btn->setCallback(callback, nullptr);                          \
	}

#define MAKE_COLOR3_BUTTON(str, width, edit, ui, type, setter)                \
	{                                                                         \
		ui->addWidget(edit = make_shared<ColorEdit>(0, 0, width, 20, str, \
													ui, m_toolset));          \
		edit->setDebugString(str); \
		edit->setCallback(                                                    \
			[this](Widget& w, void* d) {                                      \
				if (!m_sceneView->getSelection().size())                      \
					return;                                                   \
                                                                              \
				m_undo.save();                                                \
				static_cast<type*>(m_sceneView->getSelection().back())        \
					->setter(Vector3(edit->getColor()));                      \
                                                                              \
				updateSelectedObject(m_sceneView->getSelection().back());     \
			},                                                                \
			nullptr);                                                         \
	}

#define MAKE_SCENE_COLOR3_BUTTON(str, width, edit, ui, setter)                \
	{                                                                         \
		ui->addWidget(edit = make_shared<ColorEdit>(0, 0, width, 20, str, \
													ui, m_toolset));          \
		edit->setDebugString(str); \
		edit->setCallback(                                                    \
			[this](Widget& w, void* d) {                                      \
																			  \
				m_undo.save();                                                \
				NeoEngine::getInstance()->getLevel()->getCurrentScene()	      \
					->setter(Vector3(edit->getColor()));                      \
                                                                              \
			},                                                                \
			nullptr);                                                         \
	}


std::string findName(const char* name, Scene* scene)
{
	std::string base(name);
	base.erase(base.find_last_not_of("0123456789") + 1);

	std::string retval;
	retval.reserve(base.size() + 3);
	
	size_t i = 0;
	while(scene->getObjectByName((retval = base + to_string(i)).c_str())) i++;	
	return retval;
}

bool updated = false;
void EditorGame::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	float delta = engine->getGame()->getFrameDelta();

	if(!updated)
	{
		updateEntityTree();
		updated = true;
	}

	// Sleep if window is not selected
	if(!engine->getInputContext()->isKeyDown(WINDOW_SELECT))
		engine->getSystemContext()->sleep(500);

	PROFILE_BEGIN("GuiUpdate");
	m_canvas.update(delta);
	PROFILE_END("GuiUpdate");

	Vector2 const size = engine->getSystemContext()->getScreenSize();

	m_sceneView->setPosition(Vector2(m_leftPanel->getSize().x + 15, m_toolbar->getPosition().y + m_toolbar->getSize().y));
	m_sceneView->setSize(Vector2(	size.x - m_leftPanel->getSize().x - m_rightPanel->getSize().x - 30,
					size.y - 25.0f - (m_bottomPanel->isActive() ? m_bottomPanel->getSize().y : 0) - m_toolbar->getSize().y));
	
	// Update title
	//char title[64];
	//snprintf(title, sizeof(title), "%s (%d FPS)", tr("Neo Scene Editor"), static_cast<int>(floor(1/delta)));
	//engine->getSystemContext()->setWindowTitle(title);

	// Update diagnostics
	std::stringstream ss;
	ss << "Frames Per Second: " << static_cast<int>(floor(1/delta)) << std::endl;
	ss << "Latency: " << delta * 1000.0 << " ms" << std::endl;
	ss << "Visible Objects: " << *m_visibleObjectsCount << std::endl;
	m_diagnosticsLabel->setLabel(ss.str().c_str());

	// Update object UI
	if(m_sceneView->getSelection().size() && m_sceneView->getState() == Neo2D::Gui::WIDGET_SELECTED)
		updateSelectedObject(m_sceneView->getSelection().back());

	// Set values of current scene
	// updateSceneUi();
}

void EditorGame::draw()
{
	NeoEngine* engine = NeoEngine::getInstance();
	Renderer* render = engine->getRenderer();

	render->set2D(engine->getSystemContext()->getScreenSize());
	render->enableDepthTest(false);
	
	PROFILE_BEGIN("GuiDraw");
	setEditorPaths();
	m_canvas.draw();
	setProjectPaths();
	PROFILE_END("GuiDraw");
}

void EditorGame::onBegin()
{
	NeoEngine* engine = NeoEngine::getInstance();

	static Level level;
	if(engine->getLevel() == nullptr)
		engine->setLevel(&level);

	ConfigurationRegistry& registry = engine->getConfigurationRegistry();
	
#ifdef WIN32
	{
		std::string path = getenv("APPDATA");
		registry.load(path + "\\neo-editor.cfg");
	}
#else
	{
		std::string path = getenv("HOME");
		registry.load(path + "/.neo-editor.cfg");
	}
#endif
	
	m_visibleObjectsCount = registry.getVariable("g_visible_objects_count");
	m_editorPath = engine->getSystemContext()->getWorkingDirectory();
	
	// Apply some of the settings
	{
		std::string* value;
		if((value = registry.getVariable("g_editor_language")) != nullptr)
		{
			if(!Translator::loadTranslation("data/translations/" + *value + ".csv"))
				MLOG_WARNING("Could not load translation '" << *value << "'");
		}
	}
	
	// TODO: Load config!
	auto rootpane = make_shared<Container>(0, 0, 0, 0, nullptr);
	m_canvas.addObject2D(rootpane);

	m_sceneView = make_shared<SceneView>(m_undo, 0,0,0,0, rootpane);
	rootpane->addWidget(m_sceneView);

	m_menubar = make_shared<Menubar>(0, 0, 6000, 25, rootpane);
	m_toolbar = make_shared<Toolbar>(0, m_menubar->getSize().y, 6000, 32, rootpane);

	auto toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/document-new.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { openNewLevel(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/document-open.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { openLevel(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/document-save.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { saveLevel(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/edit-undo.png", m_toolbar);
	toolbutton->setCallback(
		[this](Widget&, void*) {
			undo();
		},
		nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0, 0, 32, 32, "data/icons/edit-redo.png", m_toolbar);
	toolbutton->setCallback(
		[this](Widget&, void*) {
			redo();
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

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/objects/camera.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { addCamera(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/objects/light.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { addLight(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/objects/sound.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { addSound(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/font-x-generic.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { addText(); }, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/applications-graphics.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) { addEntity(); }, nullptr);
	m_toolbar->addWidget(toolbutton);
	
	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/media-playback-start.png", m_toolbar);
	toolbutton->setCallback([this](Widget& w, void*) { 
			runGame();
		}
		, nullptr);
	
	m_toolbar->addWidget(toolbutton);
	m_playButton = toolbutton;

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/go-previous.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) {
		auto cam = m_sceneView->getCamera();
		cam->setEulerRotation(Vector3(0,0,0));
		cam->rotate(Vector3(1,0,0), 90);
		cam->rotate(Vector3(0,1,0), 90);
		cam->updateMatrix();
	}, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/go-up.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) {
		auto cam = m_sceneView->getCamera();
		cam->setEulerRotation(Vector3(-180,0,0));
		cam->updateMatrix();
	}, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/go-down.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) {
		auto cam = m_sceneView->getCamera();
		cam->setEulerRotation(Vector3(0,0,0));
		cam->updateMatrix();
	}, nullptr);
	m_toolbar->addWidget(toolbutton);

	toolbutton = make_shared<ImageButton>(0,0,32,32, "data/icons/go-next.png", m_toolbar);
	toolbutton->setCallback([this](Widget&, void*) {
		auto cam = m_sceneView->getCamera();
		cam->setEulerRotation(Vector3(0,0,0));
		cam->rotate(Vector3(1,0,0), 90);
		cam->rotate(Vector3(0,1,0), -90);
		cam->updateMatrix();		cam->updateMatrix();
	}, nullptr);
	m_toolbar->addWidget(toolbutton);

	auto filemenu = make_shared<Submenu>(tr("File"), m_menubar);
	auto editmenu = make_shared<Submenu>(tr("Edit"), m_menubar);
	auto viewmenu = make_shared<Submenu>(tr("View"), m_menubar);
	auto scenemenu = make_shared<Submenu>(tr("Scene"), m_menubar);
	auto objectmenu = make_shared<Submenu>(tr("Object"), m_menubar);
	auto helpmenu = make_shared<Submenu>(tr("Help"), m_menubar);
		
	filemenu->addItem(tr("New Level"), [this](Widget&, void*) {
		openNewLevel();
	});
	
	filemenu->addItem(tr("Open Project"), [this](Widget&, void*) {
		std::string filename = m_toolset->fileOpenDialog(tr("Open Project"), HOMEDIR, "Projects (*.nproj)");
		if(filename.empty())
			return;

		loadProject(filename.c_str());
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

	filemenu->addItem(tr("Quit"), [this] (Widget&, void*) { NeoEngine::getInstance()->setActive(false); });

   	scenemenu->addItem(tr("/Create/Light"), [this](Widget&, void*) {
			addLight();
	});

	scenemenu->addItem(tr("/Create/Entity"), [this](Widget&, void*) {
			addEntity();
	});

	scenemenu->addItem(tr("/Create/Text"), [this](Widget&, void*) {
			addText();
	});
	
	scenemenu->addItem(tr("/Create/Sound"), [this](Widget&, void*) {
			addSound();
	});

	scenemenu->addItem(tr("/Create/Camera"), [this](Widget&, void*) {
		addCamera();
	});

	scenemenu->addItem(tr("Add Scene"), [this](Widget&, void*) {
			Level* level = NeoEngine::getInstance()->getLevel();
			Scene* s = level->addNewScene();
			
			std::string name("Scene-");
			name += std::to_string(level->getScenesNumber());
			
			s->setName(name.c_str());
			level->setCurrentScene(s);
			
			updateSceneUi();

			m_sceneView->clearSelection();
			m_sceneView->updateOverlayScene();
			updated = false;
	});

	scenemenu->addItem(tr("Change Scene"), [this](Widget&, void*) {
			Level* level = NeoEngine::getInstance()->getLevel();
			std::vector<std::string> scenes;
			for(int i = 0; i < level->getScenesNumber(); i++)
				scenes.push_back(level->getSceneByIndex(i)->getName());
			
			std::string newSceneName = m_toolset->listSelection(tr("Choose a scene"), scenes);
			if(newSceneName.empty())
				return;

			level->setCurrentScene(level->getSceneByName(newSceneName.c_str()));
			updateSceneUi();

			m_sceneView->clearSelection();
			m_sceneView->updateOverlayScene();
			updated = false;
	});
	
	editmenu->addItem(tr("Undo"), [this](Widget&, void*) {
		undo();
	});

	editmenu->addItem(tr("Redo"), [this](Widget&, void*) {
		redo();
	});

	editmenu->addItem(tr("Duplicate Selection"), [this](Widget&, void*) {
		duplicateSelection();
	});

	editmenu->addItem(tr("Delete Selection"), [this](Widget&, void*) {
		deleteSelection();
	});

	editmenu->addItem(tr("Object Local Transformation"), [this](Widget& w, void*) {
			m_sceneView->setObjectLocal(!m_sceneView->isObjectLocal());

			if(m_sceneView->isObjectLocal())
				w.setLabel(tr("Selection Local Transformation"));
			else
				w.setLabel(tr("Object Local Transformation"));
	});

	editmenu->addItem(tr("Enable Snap to Grid"), [this](Widget& w, void*) {
			if(m_sceneView->getGridSize())
			{
				m_sceneView->setGridSize(0);
				w.setLabel(tr("Enable Snap to Grid"));
			}
			else
			{
				m_sceneView->setGridSize(10);
				w.setLabel(tr("Disable Snap to Grid"));
			}
		});

	editmenu->addItem(tr("Enable Snap to Ground"), [this](Widget& w, void*) {
		if(m_sceneView->isSnapToGround())
		{
			m_sceneView->enableSnapToGround(false);
			w.setLabel(tr("Enable Snap to Ground"));
		}
		else
		{
			m_sceneView->enableSnapToGround(true);
			w.setLabel(tr("Disable Snap to Ground"));
		}
	});

	viewmenu->addItem(tr("Hide Console"), [this](Widget&, void*) {
			bool newval = m_bottomPanel->isActive();
			m_bottomPanel->setActive(!newval);
			m_bottomPanel->setInvisible(newval);
		});

	viewmenu->addItem(tr("Ortho View"), [this](Widget& w, void*) {
		auto cam = m_sceneView->getCamera();
		if(cam->isOrtho())
		{
			w.setLabel(tr("Ortho View"));
			cam->enableOrtho(false);
			cam->setFov(75);
		}
		else
		{
			w.setLabel(tr("Perspective View"));
			cam->enableOrtho(true);
			cam->setEulerRotation(Vector3(0,0,0));
		}
	});

	viewmenu->addItem(tr("Wireframe Debug View"), [this](Widget&, void*) {
			NeoEngine::getInstance()->getConfigurationRegistry().setVariable("g_render_debug_mode", "wireframe");
		});

	viewmenu->addItem(tr("Wireframe + Filled Debug View"), [this](Widget&, void*) {
			NeoEngine::getInstance()->getConfigurationRegistry().setVariable("g_render_debug_mode", "fill");
		});

	viewmenu->addItem(tr("Filled View"), [this](Widget&, void*) {
			NeoEngine::getInstance()->getConfigurationRegistry().setVariable("g_render_debug_mode", "none");
		});

	objectmenu->addItem(tr("Add Behavior"), [this](Widget&, void*) {
			if(!m_sceneView->getSelection().size())
				return;

			Object3d* object = m_sceneView->getSelection().back();
			auto behaviorManager = NeoEngine::getInstance()->getBehaviorManager();
						
			std::vector<std::string> behaviors;
			for(int i = 0; i < behaviorManager->getBehaviorsNumber(); i++)
				behaviors.push_back(behaviorManager->getBehaviorByIndex(i)->getName());
			
			std::string behaviorName = m_toolset->listSelection(tr("Choose a behavior"), behaviors);
			if(behaviorName.empty())
				return;

			object->addBehavior(behaviorManager->getBehaviorByName(behaviorName.c_str())->getNewBehavior(object));			

			m_behaviorUi->displayObject(nullptr);
			updateSelectedObject(object);
		});
	
	helpmenu->addItem(tr("About"), [this](Widget&, void*) { m_toolset->aboutDialog(); });

	m_menubar->addMenu(filemenu);
	m_menubar->addMenu(editmenu);
	m_menubar->addMenu(viewmenu);
	m_menubar->addMenu(scenemenu);
	m_menubar->addMenu(objectmenu);
	m_menubar->addMenu(helpmenu);

	// Build right panel
	{
			// Right panel
			m_rightPanel = make_shared<Sidepanel>(0, m_menubar->getSize().y +
												  m_toolbar->getSize().y,
												  250, 500, rootpane);
			m_rightPanel->setEdge(RIGHT_EDGE);
			m_rightPanel->setLayout(make_shared<ScaleLayout>());

			auto rightscroll =
				make_shared<ScrollPanel>(0, 0, 0, 0, m_rightPanel);
			m_rightPanel->addWidget(rightscroll);

			float width = m_rightPanel->getSize().x - 20;

			// Object editing UIs
			m_transformUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 200, rightscroll);

			m_entityUi = make_shared<Container>(rightscroll->getPosition().x,
												   rightscroll->getPosition().y,
												   width, 200, rightscroll);

			m_physicsUi = make_shared<Container>(rightscroll->getPosition().x,
												 rightscroll->getPosition().y,
												 width, 300, rightscroll);

			m_constraintUi = make_shared<Container>(rightscroll->getPosition().x,
													rightscroll->getPosition().y,
													width, 300, rightscroll);

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

			m_sceneUi = make_shared<Container>(rightscroll->getPosition().x,
											  rightscroll->getPosition().y,
											  width, 160, rightscroll);

			m_behaviorUi = make_shared<BehaviorContainer>(rightscroll->getPosition().x,
														  rightscroll->getPosition().y,
														  width, 200, rightscroll);

			rightscroll->addWidget(m_sceneUi);
			rightscroll->addWidget(m_transformUi);
			rightscroll->addWidget(m_entityUi);
			rightscroll->addWidget(m_lightUi);
			rightscroll->addWidget(m_cameraUi);
			rightscroll->addWidget(m_soundUi);
			rightscroll->addWidget(m_textUi);
			rightscroll->addWidget(m_behaviorUi);
			
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
			m_physicsUi->setLayout(scrollLayout);
			m_constraintUi->setLayout(scrollLayout);
			m_sceneUi->setLayout(scrollLayout);
			m_behaviorUi->setLayout(scrollLayout);
			
			auto label = make_shared<Label>(0,0,0,10,tr("Name:"), m_transformUi);
			label->setColor(Vector4(0,0,0,1));
			m_transformUi->addWidget(label);
			m_transformUi->addWidget(m_nameEdit = make_shared<EditField>(0,0,width,20,nullptr,m_transformUi));

			label = make_shared<Label>(0,0,0,10,tr("Parent:"), m_transformUi);
			label->setColor(Vector4(0,0,0,1));
			m_transformUi->addWidget(label);
			m_transformUi->addWidget(m_parentEdit = make_shared<EditField>(0,0,width,20,nullptr,m_transformUi));
			
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
			
			m_parentEdit->setCallback([this](Widget& w, void* data) {

				if(!m_sceneView->getSelection().size())
					return;

				const char* name = m_parentEdit->getLabel();
				if(strlen(name) == 0) // If no name was entered, unlink
				{
					m_sceneView->getSelection().back()->unLink();
					m_undo.save();
				
					updateEntityTree();
					return;
				}

				Object3d* parent;
				if((parent = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(name)) == nullptr)
				{
					m_toolset->messagebox(tr("Error"), tr("Object not found!"));
					return;
				}

				m_sceneView->getSelection().back()->linkTo(parent);
				m_undo.save();
				
				updateEntityTree();
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

			// Entity UI
			MAKE_CHECK_BUTTON(tr("Invisible"), m_entityInvisibleButton, m_entityUi, OEntity, setInvisible);

			// Physics stuff
			{
				m_entityUi->addWidget(m_entityPhysicsButton = make_shared<CheckButton>(0, 0, 100, 20, tr("Physics"), m_entityUi));
				m_entityPhysicsButton->setCallback(
					[this](Widget&, void*) {
						if (!m_sceneView->getSelection().size())
							return;

						m_undo.save();
						if (m_entityPhysicsButton->getValue())
							static_cast<OEntity*>(
								m_sceneView->getSelection().back())
								->createPhysicsProperties();
						else
							static_cast<OEntity*>(
								m_sceneView->getSelection().back())
								->deletePhysicsProperties();

						updateSelectedObject(m_sceneView->getSelection().back());
						MLOG_INFO("Enabled/Disabled physics properties");
					},
					nullptr);

				m_physicsUi->addWidget(m_entityGhostButton = make_shared<CheckButton>(0, 0, 100, 20, tr("Ghost"), m_physicsUi));
				m_entityGhostButton->setCallback(
					[this](Widget&, void*) {
						if (!m_sceneView->getSelection().size())
							return;

						auto phys = static_cast<OEntity*>(m_sceneView->getSelection().back())->getPhysicsProperties();
						if(!phys)
							return;

						m_undo.save();
						phys->setGhost(m_entityGhostButton->getValue());
					},
					nullptr);

				m_entityUi->addWidget(m_physicsUi);

				MAKE_FLOAT_PHYSICS_EDIT_FIELD(tr("Mass"), width, m_entityMassEdit, m_physicsUi, setMass);
				MAKE_FLOAT_PHYSICS_EDIT_FIELD(tr("Friction"), width, m_entityFrictionEdit, m_physicsUi, setFriction);
				MAKE_FLOAT_PHYSICS_EDIT_FIELD(tr("Restitution"), width, m_entityRestitutionEdit, m_physicsUi, setRestitution);
				MAKE_FLOAT_PHYSICS_EDIT_FIELD(tr("Linear Damping"), width, m_entityLinearDampingEdit, m_physicsUi, setLinearDamping);
				MAKE_FLOAT_PHYSICS_EDIT_FIELD(tr("Angular Damping"), width, m_entityAngularDampingEdit, m_physicsUi, setAngularDamping);
				MAKE_FLOAT_PHYSICS_EDIT_FIELD(tr("Angular Factor"), width, m_entityAngularFactorEdit, m_physicsUi, setAngularFactor);
				MAKE_3D_PHYSICS_EDIT_FIELD(tr("Linear Factor"), width, m_entityLinearFactorEdit, m_physicsUi, setLinearFactor);

				m_entityShapeMenu = make_shared<Submenu>("", m_physicsUi);
				m_entityShapeMenu->setSize(Vector2(width, 25));
				m_entityShapeMenu->enableDrawingSelf(false);

				/*MAKE_BUTTON("Collision Shape", width, m_physicsUi, [this](Widget& w, void*) {
					bool val = !m_entityShapeMenu->isActive();
					m_entityShapeMenu->setActive(val);
					m_entityShapeMenu->setVisible(val);

					//m_entityShapeMenu->setPosition(w.getPosition() + Vector2(0, w.getSize().y));
				});*/

				m_entityMenuButton = make_shared<Button>(0, 0, width, 20, "", m_physicsUi);
				m_physicsUi->addWidget(m_entityMenuButton);

				m_entityMenuButton->setCallback([this](Widget& w, void*) {
					bool val = !m_entityShapeMenu->isActive();
					m_entityShapeMenu->setActive(val);
					m_entityShapeMenu->setVisible(val);
				}, nullptr);

				m_physicsUi->addWidget(m_entityShapeMenu);
				m_entityShapeMenu->addItem(tr("Box"), [this](Widget&, void*){
					if (!m_sceneView->getSelection().size())
						return;

					auto entity = static_cast<OEntity*>(m_sceneView->getSelection().back());
					auto phys = entity->getPhysicsProperties();
					if(!phys)
						return;

					m_undo.save();
					phys->setCollisionShape(COLLISION_SHAPE_BOX);
					
					/*Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
					PhysicsContext* physics = NeoEngine::getInstance()->getPhysicsContext();
					
					// Remove current shape from the world
					unsigned int shapeid = phys->getShapeId();
					physics->deleteShape(&shapeid);
					
					// Add the new one
					scene->prepareCollisionShape(entity);*/

					updateSelectedObject(m_sceneView->getSelection().back());
				});

				m_entityShapeMenu->addItem(tr("Cone"), [this](Widget&, void*){
					if (!m_sceneView->getSelection().size())
						return;

					auto entity = static_cast<OEntity*>(m_sceneView->getSelection().back());
					auto phys = entity->getPhysicsProperties();
					if(!phys)
						return;

					m_undo.save();
					phys->setCollisionShape(COLLISION_SHAPE_CONE);

					updateSelectedObject(m_sceneView->getSelection().back());
				});

				m_entityShapeMenu->addItem(tr("Capsule"), [this](Widget&, void*){
					if (!m_sceneView->getSelection().size())
						return;

					auto entity = static_cast<OEntity*>(m_sceneView->getSelection().back());
					auto phys = entity->getPhysicsProperties();
					if(!phys)
						return;

					m_undo.save();
					phys->setCollisionShape(COLLISION_SHAPE_CAPSULE);

					updateSelectedObject(m_sceneView->getSelection().back());
				});

				m_entityShapeMenu->addItem(tr("Cylinder"), [this](Widget&, void*){
					if (!m_sceneView->getSelection().size())
						return;

					auto entity = static_cast<OEntity*>(m_sceneView->getSelection().back());
					auto phys = entity->getPhysicsProperties();
					if(!phys)
						return;

					m_undo.save();
					phys->setCollisionShape(COLLISION_SHAPE_CYLINDER);

					updateSelectedObject(m_sceneView->getSelection().back());
				});

				m_entityShapeMenu->addItem(tr("Convex Hull"), [this](Widget&, void*){
					if (!m_sceneView->getSelection().size())
						return;

					auto entity = static_cast<OEntity*>(m_sceneView->getSelection().back());
					auto phys = entity->getPhysicsProperties();
					if(!phys)
						return;

					m_undo.save();
					phys->setCollisionShape(COLLISION_SHAPE_CONVEX_HULL);

					updateSelectedObject(m_sceneView->getSelection().back());
				});

				m_entityShapeMenu->addItem(tr("Triangle Mesh"), [this](Widget&, void*){
					if (!m_sceneView->getSelection().size())
						return;

					auto entity = static_cast<OEntity*>(m_sceneView->getSelection().back());
					auto phys = entity->getPhysicsProperties();
					if(!phys)
						return;

					m_undo.save();
					phys->setCollisionShape(COLLISION_SHAPE_TRIANGLE_MESH);

					updateSelectedObject(m_sceneView->getSelection().back());
				});

				// Constraint
				m_physicsUi->addWidget(m_entityConstraintButton = make_shared<CheckButton>(0, 0, 100, 20, tr("Constraint"), m_physicsUi));
				m_entityConstraintButton->setCallback(
					[this](Widget&, void*) {
						if (!m_sceneView->getSelection().size())
							return;

						m_undo.save();
						auto phys = static_cast<OEntity*>(
							m_sceneView->getSelection().back())->getPhysicsProperties();

						if(!phys)
							return;

						if (m_entityConstraintButton->getValue())
							phys->createConstraint();
						else
							phys->deleteConstraint();

						updateSelectedObject(m_sceneView->getSelection().back());
						MLOG_INFO("Enabled/Disabled constraint properties");
					},
					nullptr);

				MAKE_CONSTRAINT_STRING_EDIT_FIELD(tr("Parent Name:"), width, m_entityConstraintParentNameEdit, m_constraintUi, parentName);
				m_constraintUi->addWidget(m_entityDisableParentCollision = make_shared<CheckButton>(0, 0, 100, 20, tr("Parent Collision"), m_constraintUi));
				m_entityDisableParentCollision->setCallback(
					[this](Widget&, void*) {
						if (!m_sceneView->getSelection().size())
							return;

						m_undo.save();
						auto phys = static_cast<OEntity*>(
							m_sceneView->getSelection().back())->getPhysicsProperties();

						if(!phys)
							return;

						if(auto con = phys->getConstraint())
							con->disableParentCollision = !m_entityDisableParentCollision->getValue();

						updateSelectedObject(m_sceneView->getSelection().back());
					},
					nullptr);

				MAKE_3D_CONSTRAINT_EDIT_FIELD(tr("Pivot:"), width, m_entityPivotEdit, m_constraintUi, pivot);
				MAKE_3D_CONSTRAINT_EDIT_FIELD(tr("Lower Linear Limit:"), width, m_entityLowerLinearLimitEdit, m_constraintUi, lowerLinearLimit);
				MAKE_3D_CONSTRAINT_EDIT_FIELD(tr("Upper Linear Limit:"), width, m_entityUpperLinearLimitEdit, m_constraintUi, upperLinearLimit);
				MAKE_3D_CONSTRAINT_EDIT_FIELD(tr("Lower Angular Limit:"), width, m_entityLowerAngularLimitEdit, m_constraintUi, lowerAngularLimit);
				MAKE_3D_CONSTRAINT_EDIT_FIELD(tr("Upper Angular Limit:"), width, m_entityUpperAngularLimitEdit, m_constraintUi, upperAngularLimit);
				m_physicsUi->addWidget(m_constraintUi);

				/*MAKE_BUTTON("Edit Material Properties", width, m_entityUi, [](Widget& w, void* d) {
				  MLOG_INFO("Materials!");
				  });*/
			}

			// Light UI

			MAKE_COLOR3_BUTTON("", width, m_lightColorButton, m_lightUi, OLight, setColor);
			MAKE_3D_EDIT_FIELD(tr("Color:"), width, m_lightColorEdit, m_lightUi, OLight, setColor);
			MAKE_FLOAT_EDIT_FIELD(tr("Intensity:"), width, m_lightIntensityEdit, m_lightUi, OLight, setIntensity);
			MAKE_FLOAT_EDIT_FIELD(tr("Radius:"), width, m_lightRadiusEdit, m_lightUi, OLight, setRadius);
			MAKE_FLOAT_EDIT_FIELD(tr("Spot Angle:"), width, m_lightSpotAngleEdit, m_lightUi, OLight, setSpotAngle);
			MAKE_FLOAT_EDIT_FIELD(tr("Spot Exponent:"), width, m_lightSpotExponentEdit, m_lightUi, OLight, setSpotExponent);
			MAKE_CHECK_BUTTON(tr("Cast Shadows"), m_lightCastShadowButton, m_lightUi, OLight, castShadow);
			MAKE_FLOAT_EDIT_FIELD(tr("Shadow Quality:"), width, m_lightShadowQualityEdit, m_lightUi, OLight, setShadowQuality);
			MAKE_FLOAT_EDIT_FIELD(tr("Shadow Bias:"), width, m_lightShadowBiasEdit, m_lightUi, OLight, setShadowBias);
			MAKE_FLOAT_EDIT_FIELD(tr("Shadow Blur:"), width, m_lightShadowBlurEdit, m_lightUi, OLight, setShadowBlur);
			m_lightUi->updateLayout();

			// Camera UI
			MAKE_FLOAT_EDIT_FIELD(tr("Near Plane:"), width, m_cameraNearPlaneEdit, m_cameraUi, OCamera, setClippingNear);
			MAKE_FLOAT_EDIT_FIELD(tr("Far Plane:"), width, m_cameraFarPlaneEdit, m_cameraUi, OCamera, setClippingFar);
			MAKE_FLOAT_EDIT_FIELD(tr("Field of View:"), width, m_cameraFovEdit, m_cameraUi, OCamera, setFov);
			MAKE_CHECK_BUTTON(tr("Ortho"), m_cameraOrthoButton, m_cameraUi, OCamera, enableOrtho);
			MAKE_CHECK_BUTTON(tr("Fog"), m_cameraFogButton, m_cameraUi, OCamera, enableFog);
			MAKE_FLOAT_EDIT_FIELD(tr("Fog Distance:"), width, m_cameraFogDistanceEdit, m_cameraUi, OCamera, setFogDistance);
			MAKE_COLOR3_BUTTON("", width, m_cameraFogColorButton, m_cameraUi, OCamera, setFogColor);
			MAKE_3D_EDIT_FIELD(tr("Fog Color:"), width, m_cameraFogColorEdit, m_cameraUi, OCamera, setFogColor);
			MAKE_COLOR3_BUTTON("", width, m_cameraClearColorButton, m_cameraUi, OCamera, setClearColor);
			MAKE_3D_EDIT_FIELD(tr("Clear Color:"), width, m_cameraClearColorEdit, m_cameraUi, OCamera, setClearColor);
			m_cameraUi->updateLayout();

			// Sound UI
			MAKE_FLOAT_EDIT_FIELD(tr("Gain:"), width, m_soundGainEdit, m_soundUi, OSound, setGain);
			MAKE_FLOAT_EDIT_FIELD(tr("Pitch:"), width, m_soundPitchEdit, m_soundUi, OSound, setPitch);
			MAKE_FLOAT_EDIT_FIELD(tr("Radius:"), width, m_soundRadiusEdit, m_soundUi, OSound, setRadius);
			MAKE_FLOAT_EDIT_FIELD(tr("Rolloff:"), width, m_soundRolloffEdit, m_soundUi, OSound, setRolloff);
			MAKE_CHECK_BUTTON(tr("Looping"), m_soundLoopingButton, m_soundUi, OSound, setLooping);
			MAKE_CHECK_BUTTON(tr("Relative"), m_soundRelativeButton, m_soundUi, OSound, setRelative);
			MAKE_BUTTON(tr("Play/Stop"), width, m_soundUi, [this](Widget& w, void*) {
				if(!m_sceneView->getSelection().size())
					return;

				OSound* snd = static_cast<OSound*>(m_sceneView->getSelection().back());
				if(snd->isPlaying()) snd->stop();
				else snd->play();
			});
			m_soundUi->updateLayout();

			// Text UI
			MAKE_STRING_EDIT_FIELD(tr("Text:"), width, m_textTextEdit, m_textUi, OText, setText);
			MAKE_FLOAT_EDIT_FIELD(tr("Size:"), width, m_textSizeEdit, m_textUi, OText, setSize);
			MAKE_COLOR3_BUTTON("", width, m_textColorButton, m_textUi, OText, setColor);
			MAKE_4D_EDIT_FIELD(tr("Color:"), width, m_textColorEdit, m_textUi, OText, setColor);
			m_textUi->updateLayout();

			// Scene UI
			MAKE_SCENE_STRING_EDIT_FIELD(tr("Scene Name:"), width, m_sceneNameEdit, m_sceneUi, setName);
			MAKE_SCENE_STRING_EDIT_FIELD(tr("Script File:"), width, m_sceneScriptFileEdit, m_sceneUi, setScriptFilename);
			MAKE_3D_SCENE_EDIT_FIELD(tr("Gravity:"), width, m_sceneGravityEdit, m_sceneUi, setGravity);

			MAKE_LABEL(tr("Ambient Light:"), m_sceneUi);
			MAKE_SCENE_COLOR3_BUTTON("", width, m_sceneAmbientLightEdit, m_sceneUi, setAmbientLight);

			// Set layout
			m_rightPanel->update(0);

			// Hide UI initially
			m_transformUi->activateChildren(false);
			m_transformUi->setInvisible(true);

			m_entityUi->activateChildren(false);
			m_entityUi->setInvisible(true);

			m_lightUi->activateChildren(false);
			m_lightUi->setInvisible(true);

			m_cameraUi->activateChildren(false);
			m_cameraUi->setInvisible(true);

			m_soundUi->activateChildren(false);
			m_soundUi->setInvisible(true);

			m_textUi->activateChildren(false);
			m_textUi->setInvisible(true);

			m_behaviorUi->activateChildren(false);
			m_behaviorUi->setInvisible(true);
			
			// Show scene UI when everything else is hidden
			m_sceneUi->setActive(true);
			m_sceneUi->setInvisible(false);
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
	
	// Build bottom panel
	{
		// Bottom panel
		m_bottomPanel = make_shared<Sidepanel>(m_leftPanel->getSize().x + 15, 0,
											   0, 200, rootpane);

		m_bottomPanel->setEdge(BOTTOM_EDGE);
		m_bottomPanel->setLayout(make_shared<ScaleLayout>());
		m_bottomPanel->setSizeOffset(Vector2(-m_rightPanel->getSize().x, 0));
		
		m_console = make_shared<Console>(0,0,0,0, m_bottomPanel);

		m_bottomPanel->addWidget(m_console);
	}
		
	m_diagnosticsLabel = make_shared<Label>(m_leftPanel->getSize().x + 20, 10 + m_toolbar->getPosition().y + m_toolbar->getSize().y, 0, 0, "DIAGNOSTICS", rootpane);
	rootpane->addWidget(m_diagnosticsLabel);

	rootpane->addWidget(m_bottomPanel);
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
						if(m_sceneView->getSelection().size())
						{
							m_entityTree->setSelected(m_sceneView->getSelection().back()->getName());
							updateSelectedObject(m_sceneView->getSelection().back());
						}
						else updateSelectedObject(nullptr);
					}, nullptr);

	m_keyboardShortcuts = make_shared<KeyboardShortcuts>(rootpane);
	rootpane->addWidget(m_keyboardShortcuts);

	// TODO: Event priorities
	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_S}, [this](void*){
		saveLevel();
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_O}, [this](void*){
		openLevel();
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_1}, [this](void*){
		m_sceneView->setHandleMode(TRANSLATION);
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_2}, [this](void*){
		m_sceneView->setHandleMode(SCALE);
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_3}, [this](void*){
		m_sceneView->setHandleMode(ROTATION);
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_DELETE}, [this](void*){
		deleteSelection();
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_D}, [this](void*){
		duplicateSelection();
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_Z}, [this](void*){
		undo();
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({KEY_LCONTROL, KEY_LSHIFT, KEY_Z}, [this](void*){
		redo();
	}, nullptr));
	
	m_keyboardShortcuts->addShortcut(Shortcut({KEY_F5}, [this](void*){
		runGame();
	}, nullptr));

	m_keyboardShortcuts->addShortcut(Shortcut({WINDOW_SELECT}, [this](void*){
		Level* level = NeoEngine::getInstance()->getLevel();
		level->getTextureManager()->update();
		level->getShaderManager()->update();
		level->getSoundManager()->update();
		level->getMeshManager()->update();
		level->getArmatureAnimManager()->update();
		level->getTexturesAnimManager()->update();
		level->getMaterialsAnimManager()->update();
	}, nullptr));

	SystemContext* system = engine->getSystemContext();

	system->setWindowTitle(tr("Neo Scene Editor"));
	engine->getGame()->setDrawMainScene(false);

	char** argv = system->getArgv();
	for(int i = 0; i < system->getArgc(); i++)
	{
		if(!strcmp(argv[i], "-p") && i < system->getArgc() - 1)
		{
			loadProject(argv[i+1]);
			break;
		}
	}

	if(level.getScenesNumber() == 0)
		engine->loadLevel("");
}

void EditorGame::onEnd()
{
	m_sceneView->clear();
	Neo2D::Neo2DLevel::getInstance()->clear();
	
#ifdef WIN32
	{
		std::string path = getenv("APPDATA");
		NeoEngine::getInstance()->getConfigurationRegistry().save(path + "\\neo-editor.cfg");
	}
#else
	{
		std::string path = getenv("HOME");
		NeoEngine::getInstance()->getConfigurationRegistry().save(path + "/.neo-editor.cfg");
	}
#endif
}

void EditorGame::updateEntityTree()
{
	NeoEngine* engine = NeoEngine::getInstance();
	auto scene = engine->getLevel()->getCurrentScene();
	auto system = engine->getSystemContext();

	// Make sure the editor finds all fonts when a project is loaded
	std::string backupPath = system->getWorkingDirectory();
	system->setWorkingDirectory("./");

	std::function<void(TreeView::TreeNode*, Object3d*)> updateChildren = [&updateChildren](TreeView::TreeNode* node, Object3d* object)
	{		
		for(int i = 0; i < object->getChildrenNumber(); i++)
		{			
			if(strlen(object->getChild(i)->getName()) == 0)
				continue;

			updateChildren(node->addChild(object->getChild(i)->getName()).get(), object->getChild(i));
		}
	};

	m_entityTree->getRoot()->clear();
	for(int i = 0; i < scene->getObjectsNumber(); i++)
	{
		Object3d* object = scene->getObjectByIndex(i);
		
		if(!object->hasParent()) // Only work on top-level entities at this stage
			updateChildren(m_entityTree->getRoot()->addChild(object->getName()).get(), object);
	}

	m_sceneView->updateOverlayScene();
	system->setWorkingDirectory(backupPath.c_str());
}

void EditorGame::updateSelectedObject(Neo::Object3d* object)
{
	m_entityUi->activateChildren(false);
	m_entityUi->setInvisible(true);
	m_entityUi->setSize(m_entityUi->calculateContentSize());

	m_lightUi->activateChildren(false);
	m_lightUi->setInvisible(true);
	m_lightUi->setSize(m_lightUi->calculateContentSize());

	m_cameraUi->activateChildren(false);
	m_cameraUi->setInvisible(true);
	m_cameraUi->setSize(m_cameraUi->calculateContentSize());
	
	m_soundUi->activateChildren(false);
	m_soundUi->setInvisible(true);
	m_soundUi->setSize(m_soundUi->calculateContentSize());
	
	m_textUi->activateChildren(false);
	m_textUi->setInvisible(true);
	m_textUi->setSize(m_textUi->calculateContentSize());
	
	m_transformUi->activateChildren(false);
	m_transformUi->setInvisible(true);

	m_behaviorUi->activateChildren(false);
	m_behaviorUi->setInvisible(true);
	m_behaviorUi->setSize(Vector2(m_rightPanel->getSize().x - 15.0f, m_behaviorUi->calculateContentSize().y));

	if(!object)
		return;

	m_nameEdit->setLabel(object->getName());
	m_nameEdit->setCaret(0);

	m_positionEdit->setVector(object->getPosition());
	m_rotationEdit->setVector(object->getEulerRotation());
	m_scaleEdit->setVector(object->getScale());

	m_behaviorUi->activateChildren(true);
	m_behaviorUi->setInvisible(false);
	
	setEditorPaths(); // Ensure that font loading works
	m_behaviorUi->displayObject(object);
	setProjectPaths();
	
	switch(object->getType())
	{
	case OBJECT3D_ENTITY: {
		m_entityUi->activateChildren(true);
		m_entityUi->setInvisible(false);

		m_transformUi->activateChildren(true);
		m_transformUi->setInvisible(false);

		OEntity* entity = static_cast<OEntity*>(object);
		m_entityInvisibleButton->setValue(entity->isInvisible());

		PhysicsProperties* phys = entity->getPhysicsProperties();
		if(phys)
		{
			m_entityPhysicsButton->setValue(true);
			m_entityGhostButton->setValue(phys->isGhost());
			m_entityMassEdit->setLabel(std::to_string(phys->getMass()).c_str());
			m_entityFrictionEdit->setLabel(std::to_string(phys->getFriction()).c_str());
			m_entityRestitutionEdit->setLabel(std::to_string(phys->getRestitution()).c_str());
			m_entityLinearDampingEdit->setLabel(std::to_string(phys->getLinearDamping()).c_str());
			m_entityAngularDampingEdit->setLabel(std::to_string(phys->getAngularDamping()).c_str());
			m_entityAngularFactorEdit->setLabel(std::to_string(phys->getAngularFactor()).c_str());
			m_entityLinearFactorEdit->setVector(*phys->getLinearFactor());

			m_physicsUi->setInvisible(false);
			m_physicsUi->activateChildren(true);
			m_physicsUi->setSize(m_physicsUi->calculateContentSize());

			switch(phys->getCollisionShape())
			{
				case COLLISION_SHAPE_BOX:
					m_entityMenuButton->setLabel(tr("Box"));
					break;
				case COLLISION_SHAPE_SPHERE:
					m_entityMenuButton->setLabel(tr("Sphere"));
					break;
				case COLLISION_SHAPE_CONE:
					m_entityMenuButton->setLabel(tr("Cone"));
					break;
				case COLLISION_SHAPE_CAPSULE:
					m_entityMenuButton->setLabel(tr("Capsule"));
					break;
				case COLLISION_SHAPE_CYLINDER:
					m_entityMenuButton->setLabel(tr("Cylinder"));
					break;
				case COLLISION_SHAPE_CONVEX_HULL:
					m_entityMenuButton->setLabel(tr("Convex Hull"));
					break;
				case COLLISION_SHAPE_TRIANGLE_MESH:
					m_entityMenuButton->setLabel(tr("Triangle Mesh"));
					break;
			}

			auto con = phys->getConstraint();
			if(con)
			{
				m_entityConstraintButton->setValue(true);
				m_constraintUi->setInvisible(false);
				m_constraintUi->activateChildren(true);
				m_constraintUi->setSize(m_constraintUi->calculateContentSize());

				m_entityConstraintParentNameEdit->setLabel(con->parentName.getSafeString());
				m_entityPivotEdit->setVector(con->pivot);
				m_entityLowerLinearLimitEdit->setVector(con->lowerLinearLimit);
				m_entityLowerAngularLimitEdit->setVector(con->lowerAngularLimit);
				m_entityUpperLinearLimitEdit->setVector(con->upperLinearLimit);
				m_entityUpperAngularLimitEdit->setVector(con->upperAngularLimit);
				m_entityDisableParentCollision->setValue(!con->disableParentCollision);
			}
			else
			{
				m_entityConstraintButton->setValue(false);
				m_constraintUi->setInvisible(true);
				m_constraintUi->activateChildren(false);
			}
		}
		else
		{
			m_entityPhysicsButton->setValue(false);
			m_physicsUi->setInvisible(true);
			m_physicsUi->activateChildren(false);
		}
	}
	break;

	case OBJECT3D_LIGHT: {
		m_lightUi->activateChildren(true);
		m_lightUi->setInvisible(false);

		m_transformUi->activateChildren(true);
		m_transformUi->setInvisible(false);

		OLight* light = static_cast<OLight*>(object);
		m_lightColorButton->setColor(light->getColor());
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
		m_cameraUi->activateChildren(true);
		m_cameraUi->setInvisible(false);

		m_transformUi->activateChildren(true);
		m_transformUi->setInvisible(false);

		OCamera* cam = static_cast<OCamera*>(object);
		m_cameraFarPlaneEdit->setLabel(std::to_string(cam->getClippingFar()).c_str());
		m_cameraNearPlaneEdit->setLabel(std::to_string(cam->getClippingNear()).c_str());
		m_cameraFovEdit->setLabel(std::to_string(cam->getFov()).c_str());
		m_cameraFogButton->setValue(cam->hasFog());
		m_cameraOrthoButton->setValue(cam->isOrtho());
		m_cameraClearColorEdit->setVector(cam->getClearColor());
		m_cameraFogColorButton->setColor(cam->getFogColor());
		m_cameraFogColorEdit->setVector(cam->getFogColor());
		m_cameraFogDistanceEdit->setLabel(std::to_string(cam->getFogDistance()).c_str());
		m_cameraClearColorEdit->setVector(cam->getClearColor());
		m_cameraClearColorButton->setColor(cam->getClearColor());
	}
	break;
	
	case OBJECT3D_SOUND: {
		m_soundUi->activateChildren(true);
		m_soundUi->setInvisible(false);

		m_transformUi->activateChildren(true);
		m_transformUi->setInvisible(false);

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
		m_textUi->activateChildren(true);
		m_textUi->setInvisible(false);

		m_transformUi->activateChildren(true);
		m_transformUi->setInvisible(false);

		OText* text = static_cast<OText*>(object);
		m_textTextEdit->setLabel(text->getText());
		m_textColorEdit->setVector(text->getColor());
		m_textColorButton->setColor(text->getColor());
		m_textSizeEdit->setLabel(std::to_string(text->getSize()).c_str());
	}
	break;
	}
}

void EditorGame::openLevel()
{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		std::string filename = m_toolset->fileOpenDialog(tr("Open Level"), engine->getSystemContext()->getWorkingDirectory(), "Levels (*.level, *.llvl)");
		if(filename.empty())
			return;

		openLevel(filename.c_str());
}

void EditorGame::openLevel(const char* path)
{
		
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
		m_sceneView->clearOverlayScene();

		engine->getLevel()->clear();
		engine->getLevelLoader()->loadData(path, engine->getLevel());

		updated = false;

		m_undo.clear();
		m_undo.save();

		Scene* scene = engine->getLevel()->getCurrentScene();
		scene->enablePhysicsSimulation(false);
		
		m_currentLevelFile = path;
		updateWindowTitle();
		updateSceneUi();
}

void EditorGame::saveLevel()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	if(m_currentLevelFile.empty())
        m_currentLevelFile = m_toolset->fileSaveDialog(tr("Save Level"), engine->getSystemContext()->getWorkingDirectory(), "Lisp Levels (*.llvl)");

	if(m_currentLevelFile.empty())
		return;

	if(!m_currentProjectFile.empty())
	{
		char local[256];
		getLocalFilename(local, engine->getSystemContext()->getWorkingDirectory(), m_currentLevelFile.c_str());
		m_project.setLevel(local);
		m_project.save(m_currentProjectFile.c_str());
	}
	
	engine->getLevelLoader()->saveData(m_currentLevelFile.c_str(), "llvl", engine->getLevel());
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
	ss << tr("Neo Scene Editor") << " - " << ((m_currentLevelFile.empty()) ? tr("New Level") : m_currentLevelFile);
	engine->getSystemContext()->setWindowTitle(ss.str().c_str());
}

// TODO: Should be visible to the player as well
// Move it to NeoEngine or NeoGame?
void EditorGame::loadProject(const char* path)
{
	MLOG_INFO("Loading project file: " << path);	
	if(!m_project.load(path))
	{
		m_toolset->messagebox(tr("Error"), tr("Could not open project! Make sure the file exists and can be read from."));
		return;
	}
	
	m_currentProjectFile = path;

	char dir[256];
	getRepertory(dir, path);

	SystemContext* system = NeoEngine::getInstance()->getSystemContext();
	system->setWorkingDirectory(dir);
	
	m_projectPath = dir;

	if(!m_project.getLevel().empty())
	{
		std::string absolutePath(dir);
		openLevel((absolutePath + m_project.getLevel()).c_str());
	}
	else
		updateWindowTitle();

}

void EditorGame::saveProject(const char* path)
{
	m_project.save(path);
}

void EditorGame::addLight()
{
	OLight* light = NeoEngine::getInstance()->getLevel()->getCurrentScene()->addNewLight();

	if (m_sceneView->getSelection().size())
		light->setName(findName(m_sceneView->getSelection().back()->getName(),
								NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());
	else
		light->setName(findName("Light", NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());

	light->setPosition(m_sceneView->getSelectionCenter());

	m_sceneView->updateOverlayScene();
	m_sceneView->clearSelection();
	m_sceneView->addSelectedObject(light);
	updateSelectedObject(light);
	updated = false;
}

void EditorGame::addCamera()
{
	OCamera* camera = NeoEngine::getInstance()->getLevel()->getCurrentScene()->addNewCamera();

	if (m_sceneView->getSelection().size())
		camera->setName(findName(m_sceneView->getSelection().back()->getName(),
								NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());
	else
		camera->setName(findName("Camera", NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());

	camera->setPosition(m_sceneView->getSelectionCenter());

	m_sceneView->updateOverlayScene();
	m_sceneView->clearSelection();
	m_sceneView->addSelectedObject(camera);
	updateSelectedObject(camera);
	updated = false;
}

void EditorGame::addSound()
{
	std::string file = m_toolset->fileOpenDialog(
		tr("Open Sound File"),
		NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(),
        "All Sounds (*.ogg, *.wav);;OGG Sound Files (*.ogg);;WAV Sound Files (*.wav)");

	if(file.empty())
		return;

	Level* level = NeoEngine::getInstance()->getLevel();
	OSound* sound = level->getCurrentScene()->addNewSound(level->loadSound(file.c_str()));

	if (m_sceneView->getSelection().size())
		sound->setName(findName(m_sceneView->getSelection().back()->getName(),
								NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());
	else
		sound->setName(findName("Sound", NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());


	sound->setPosition(m_sceneView->getSelectionCenter());

	m_sceneView->updateOverlayScene();
	m_sceneView->clearSelection();
	m_sceneView->addSelectedObject(sound);
	updateSelectedObject(sound);
	updated = false;
}

void EditorGame::addEntity()
{
	std::string file = m_toolset->fileOpenDialog(
		tr("Open Mesh File"),
		NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(),
        "Mesh Files (*.obj, *.3ds, *.fbx, *.dae, *.ase, *.ifc, *.ply, *.dxf, *.lwo, *.lws, "
            "*.lxo, *.stl, *.x, *.ac, *.ms3d, *.cob, *.irrmesh, *.irr, *.md1, *.md2, *.md3, *.pk3, "
            "*.mdc, *.md5, *.smd, *.ogex, *.3d, *.b3d, *.q3d, *.mesh)");

	if(file.empty())
		return;

	Level* level = NeoEngine::getInstance()->getLevel();
	OEntity* entity = level->getCurrentScene()->addNewEntity(level->loadMesh(file.c_str()));

	if (m_sceneView->getSelection().size())
		entity->setName(findName(m_sceneView->getSelection().back()->getName(),
								 NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());
	else
		entity->setName(findName("Entity", NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());


	entity->setPosition(m_sceneView->getSelectionCenter());

	m_sceneView->updateOverlayScene();
	m_sceneView->clearSelection();
	m_sceneView->addSelectedObject(entity);
	updateSelectedObject(entity);
	updated = false;
}

void EditorGame::addText()
{
	std::string file = m_toolset->fileOpenDialog(
		tr("Open Font File"),
		NeoEngine::getInstance()->getSystemContext()->getWorkingDirectory(),
		"Font Files (*.ttf)");

	if(file.empty())
		return;

	Level* level = NeoEngine::getInstance()->getLevel();
	OText* text = level->getCurrentScene()->addNewText(level->loadFont(file.c_str()));

	if (m_sceneView->getSelection().size())
		text->setName(findName(m_sceneView->getSelection().back()->getName(),
							   NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());
	else
		text->setName(findName("Text", NeoEngine::getInstance()->getLevel()->getCurrentScene()).c_str());


	text->setPosition(m_sceneView->getSelectionCenter());

	m_sceneView->updateOverlayScene();
	m_sceneView->clearSelection();
	m_sceneView->addSelectedObject(text);
	updateSelectedObject(text);
	updated = false;
}

void EditorGame::openNewLevel()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	std::string path =  m_toolset->fileSaveDialog(tr("Save Level"), engine->getSystemContext()->getWorkingDirectory(), "Lisp Levels (*.llvl)");

	if(path.empty())
		return;

	m_currentLevelFile = path;
	m_sceneView->clearSelection();
	m_sceneView->clearOverlayScene();

	engine->getLevel()->clear();

	Level empty;
	auto mainscene = empty.addNewScene();
	mainscene->setName("Scene-1");

	engine->getLevelLoader()->saveData(m_currentLevelFile.c_str(), "llvl", &empty);
	engine->getLevelLoader()->loadData(m_currentLevelFile.c_str(), engine->getLevel());

	engine->getLevel()->getCurrentScene();
	m_sceneView->updateOverlayScene();

	updated = false;
	updateWindowTitle();
}

void EditorGame::deleteSelection()
{
	auto scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

	for(auto v : m_sceneView->getSelection())
		scene->deleteObject(v);

	m_sceneView->getSelection().clear();
	m_sceneView->updateOverlayScene();
	updated = false;

	m_undo.save();
}

void EditorGame::duplicateSelection()
{
	std::function<Object3d*(Object3d*, Scene*)> duplicateObject = [](Object3d* object, Scene* scene) {
		Object3d* clone;
		switch(object->getType())
		{
			case OBJECT3D_LIGHT:
				clone = scene->addNewLight(static_cast<const OLight&>(*object));
				break;

			case OBJECT3D_SOUND:
				clone = scene->addNewSound(static_cast<const OSound&>(*object));
				break;

			case OBJECT3D_CAMERA:
				clone = scene->addNewCamera(static_cast<const OCamera&>(*object));
				break;

			case OBJECT3D_TEXT:
				clone = scene->addNewText(static_cast<const OText&>(*object));
				break;

			case OBJECT3D_ENTITY:
				clone = scene->addNewEntity(static_cast<const OEntity&>(*object));
				break;
		}

		clone->setName(findName(object->getName(), scene).c_str());
		return clone;
	};

	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
	std::vector<Object3d*> newSelection;

	for(auto object : m_sceneView->getSelection())
		newSelection.push_back(duplicateObject(object, scene));

	m_sceneView->clearSelection();
	m_sceneView->getSelection() = std::move(newSelection);
	m_undo.save();

	updated = false;
	m_sceneView->updateOverlayScene();
}

void EditorGame::undo()
{
	if(m_disableUndo)
		return;
	
	m_sceneView->clearSelection();
	m_undo.undo();
	m_sceneView->updateOverlayScene();
	
	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
	scene->enablePhysicsSimulation(false);
	
	updateEntityTree();
}

void EditorGame::redo()
{	
	if(m_disableUndo)
		return;
	
	m_sceneView->clearSelection();
	m_undo.redo();
	m_sceneView->updateOverlayScene();

	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
	scene->enablePhysicsSimulation(false);
	
	updateEntityTree();
}

void EditorGame::runGame()
{
	NeoEngine* engine = NeoEngine::getInstance();
	if(m_isRunningGame)
	{
		engine->setActive(false);
		return;
	}

	// Save current state
	m_undo.save();

	// Disable undo/redo queue so it doesn't corrupt
	m_disableUndo = true;

	Neo::Player player(16);
	m_sceneView->clearSelection();

	ScriptContext* script = engine->getScriptContext();
	
	char scriptFile[256]; // Get global file path
	getGlobalFilename(scriptFile, engine->getSystemContext()->getWorkingDirectory(), 
				engine->getLevel()->getCurrentScene()->getScriptFilename());
	
	script->runScript(scriptFile);
	
	setEditorPaths(); // Ensure that the file path is relative to the editor
	m_playButton.lock()->loadImage("data/icons/media-playback-pause.png");
	setProjectPaths();
	
	m_isRunningGame = true;
	m_sceneView->showEditorScenes(false);
	
	Scene* scene = engine->getLevel()->getCurrentScene();
	scene->preparePhysics();
	scene->enablePhysicsSimulation(true);	
	
	player.execute(KEY_ESCAPE);
	
	m_isRunningGame = false;
	m_sceneView->showEditorScenes(true);
	
	setEditorPaths();
	m_playButton.lock()->loadImage("data/icons/media-playback-start.png");
	setProjectPaths();
		
	// Re-enable undo/redo queue again
	m_disableUndo = false;
	
	// Restore original state
	undo();
	
	// Turn off physics
	scene = engine->getLevel()->getCurrentScene();
	scene->enablePhysicsSimulation(false);
	
	// Rest input handling
	engine->getInputContext()->setMouseRelative(false);
	engine->getSystemContext()->showCursor();
	
	engine->setActive(true);
	script->stopRunning();
}

void EditorGame::updateSceneUi()
{
	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
	m_sceneNameEdit->setLabel(scene->getName());
	m_sceneScriptFileEdit->setLabel((scene->getScriptFilename()) ? scene->getScriptFilename() : "");
	m_sceneGravityEdit->setVector(scene->getGravity());
	m_sceneAmbientLightEdit->setColor(scene->getAmbientLight());
}


void EditorGame::setEditorPaths()
{
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();
	system->setWorkingDirectory(m_editorPath.c_str());
}

void EditorGame::setProjectPaths()
{
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();
	system->setWorkingDirectory(m_projectPath.c_str());
}


