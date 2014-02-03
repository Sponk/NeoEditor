/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// Maratis.cpp
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


#include <stdio.h>

// GL, TODO : remove opengl call (should use MRenderingContext to be fully virtual)
#ifdef _WIN32
	#include <GLee.h>
#elif __APPLE__
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
#elif linux
	#include <GLee.h>
#endif

#include "MaratisUI.h"

// Maratis Common
#include <MContexts/MGLContext.h>
#include <MContexts/MALContext.h>
#include <MContexts/MBulletContext.h>
#include <MContexts/MWinContext.h>
#include <MLoaders/MDevILLoader.h>
#include <MLoaders/MSndFileLoader.h>
#include <MLoaders/MFreetypeLoader.h>
#include <MLoaders/MBinFontLoader.h>
#include <MLoaders/MBinMeshLoader.h>

#include <MBehaviors/MBLookAt.h>
#include <MBehaviors/MBFollow.h>
#include <MScript/MScript.h>
#include <MInput/MInput.h>
#include <MFileManager/MLevelLoad.h>
#include <MFileManager/MLevelSave.h>
#include <MFileManager/MMeshLoad.h>
#include <MFileManager/MMeshSave.h>
#include <MFileManager/MPackageManagerNPK.h>
#include <MProject/MProject.h>
#include <MRenderers/MStandardRenderer.h>
#include <MRenderers/MFixedRenderer.h>

// Bins
#include "../MBins/MFontBin.h"
#include "../MBins/MMeshBin.h"

// publisher
#include "../MPublish/MPublisher.h"

// assimp loader
#include "../MLoaders/MAssimpMeshLoader.h"


// add ext if not
void fileExtension(char * out, const char * in, const char * ext)
{
	int extLen = strlen(ext);

	int extId = strlen(in) - extLen;
	if(extId < 0)
		extId = 0;

	char copy[256];

	// *.level
	if(strncmp(in + extId, ext, extLen))
	{
		sprintf(copy, "%s%s", in, ext);
		strcpy(out, copy);
	}
	else
	{
		strcpy(out, in);
	}
}


// Skin cache
class MSkinCache
{
private:

	unsigned int m_verticesNumber;
	unsigned int m_normalsNumber;
	MVector3 * m_vertices;
	MVector3 * m_normals;

public:

	MSkinCache(void):
    m_verticesNumber(0),
    m_normalsNumber(0),
    m_vertices(NULL),
    m_normals(NULL)
	{}

	virtual ~MSkinCache(void)
	{
		SAFE_DELETE_ARRAY(m_vertices);
		SAFE_DELETE_ARRAY(m_normals);
	}

	static MSkinCache * getInstance(void)
	{
		static MSkinCache m_instance;
		return &m_instance;
	};

	MVector3 * getVertices(unsigned int size)
	{
		if(size > m_verticesNumber)
		{
			SAFE_DELETE_ARRAY(m_vertices);
			m_vertices = new MVector3[size];
			m_verticesNumber = size;
		}

		return m_vertices;
	}

	MVector3 * getNormals(unsigned int size)
	{
		if(size > m_normalsNumber)
		{
			SAFE_DELETE_ARRAY(m_normals);
			m_normals = new MVector3[size];
			m_normalsNumber = size;
		}

		return m_normals;
	}
};








Maratis::Maratis(void):
m_firstUndo(false),
m_maxUndo(32),
m_undo(0),
m_undoNumber(0),
m_undoLevel(0),
m_timelinePos(0),
m_timelineMin(0),
m_timelineMax(240),
m_gamePlugin(NULL),
m_sphereEntity(NULL),
m_coneEntity(NULL),
m_cubeEntity(NULL),
m_xEntity(NULL),
m_yEntity(NULL),
m_zEntity(NULL),
m_xcircleEntity(NULL),
m_ycircleEntity(NULL),
m_zcircleEntity(NULL),
m_lightEntity(NULL),
m_cameraEntity(NULL),
m_soundEntity(NULL),
m_emptyText(NULL),
m_renderer(NULL)
{
	// tempDir
	{
		MWindow * window = MWindow::getInstance();

		unsigned int i = 1;
		sprintf(m_tempDir, "%s/mtemp%d", window->getTempDirectory(), i);
		while(isDirectory(m_tempDir))
		{
			i++;
			sprintf(m_tempDir, "%s/mtemp%d", window->getTempDirectory(), i);
		}
		
		createDirectory(m_tempDir);
	}

	// MEngine
	{
		m_soundContext = new MALContext();
		m_render = new MGLContext();
		m_physics = new MBulletContext();
		m_script = new MScript();
		m_input = new MInput();
		m_system = new MWinContext();
		m_level = new MLevel();
		m_game = new MGame();
		m_packageManager = new MPackageManagerNPK();
		
		m_physics->setSimulationQuality(2);
	}

	// start
	start();

	// empty text
	m_emptyText.setText("T");

	// transform
	m_transformMode = M_TRANSFORM_POSITION;
	m_orientationMode = M_ORIENTATION_WORLD;
	m_currentAxis = M_AXIS_NONE;

	initVue();
}

Maratis::~Maratis(void)
{
	clear();
	removeDirectory(m_tempDir);

	SAFE_DELETE(m_game);
	SAFE_DELETE(m_level);

	SAFE_DELETE(m_render);
	SAFE_DELETE(m_soundContext);
	SAFE_DELETE(m_physics);
	SAFE_DELETE(m_script);
	SAFE_DELETE(m_input);
	SAFE_DELETE(m_system);
	SAFE_DELETE(m_packageManager);
}

void Maratis::publish(void)
{
	MaratisUI * UI = MaratisUI::getInstance();
	
	if(strcmp(m_currentProject, "") != 0)
	{
		MPublisher * publisher = MPublisher::getInstance();
		publisher->publish(m_currentProject);
	}
	else
	{
		UI->setPopupButton("you need to open a project to publish", NULL);
	}
}

void Maratis::changeRenderer(const char * name)
{
	MEngine * engine = MEngine::getInstance();
	MRendererManager * rendererManager = engine->getRendererManager();

	MRendererCreator * renderer = rendererManager->getRendererByName(name);
	if(renderer)
	{
		if(m_renderer)
			m_renderer->destroy();
		m_renderer = renderer->getNewRenderer();
		engine->setRenderer(m_renderer);
	}
}

void Maratis::autoSave(void)
{
	MEngine * engine = MEngine::getInstance();


	// autosave name
	char autoSaveName[64];
	sprintf(autoSaveName, "%d.level", m_undoLevel);

	// save auto save
	const char * temp = getTempDir();
	if(! temp)
		return;

	string tempFile(temp);
	tempFile += "/";
	tempFile += autoSaveName;
	xmlLevelSave(engine->getLevel(), tempFile.c_str());

	// increment undo
	m_undoLevel++;
	m_undo++;

	if(m_undoLevel >= m_maxUndo)
		m_undoLevel = 0;

	if(m_undo > m_maxUndo)
		m_undo = m_maxUndo;

	m_undoNumber = m_undo;

	m_firstUndo = true;

	if(m_currentLevel[0] != '\0')
		updateTitle(" *");
}

void Maratis::undo(void)
{
	if((m_undoNumber == 0) || (m_undo == 0))
		return;

	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	// auto save if first undo
	if(m_firstUndo)
	{
		autoSave();
		m_firstUndo = false;

		if(m_undoLevel > 0)
			m_undoLevel--;
		else
			m_undoLevel = m_maxUndo-1;

		m_undo--;
	}

	// decrement undo
	if(m_undoLevel > 0)
		m_undoLevel--;
	else
		m_undoLevel = m_maxUndo-1;

	m_undo--;

	// autosave name
	char autoSaveName[64];
	sprintf(autoSaveName, "%d.level", m_undoLevel);

	// load auto save
	string tempFile(getTempDir());
	tempFile += "/";
	tempFile += autoSaveName;
	M_loadLevel(tempFile.c_str(), level, false);

	// update interface
	clearSelectedObjects();
	UI->editObject(NULL);
	UI->updateSceneMenu();
	engine->getLevel()->getCurrentScene()->updateObjectsMatrices();
}

void Maratis::redo(void)
{
	if((m_undo+1) >= m_undoNumber)
		return;

	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	// increment undo
	m_undoLevel++;
	m_undo++;

	if(m_undoLevel >= m_maxUndo)
		m_undoLevel = 0;

	// autosave name
	char autoSaveName[64];
	sprintf(autoSaveName, "%d.level", m_undoLevel);

	// load auto save
	string tempFile(getTempDir());
	tempFile += "/";
	tempFile += autoSaveName;
	M_loadLevel(tempFile.c_str(), level, false);

	// update interface
	clearSelectedObjects();
	UI->editObject(NULL);
	UI->updateSceneMenu();
	engine->getLevel()->getCurrentScene()->updateObjectsMatrices();
}

void Maratis::clearUndo(void)
{
	m_undo = 0;
	m_undoLevel = 0;
	m_undoNumber = 0;
}

MMeshRef * Maratis::loadEditorMesh(const char * filename)
{
	MMesh * mesh = MMesh::getNew();
	if(xmlMeshLoad(filename, mesh))
	{
		MMeshRef * ref = MMeshRef::getNew(mesh, filename);
		m_meshManager.addRef(ref);
		return ref;
	}

	mesh->destroy();
	return NULL;
}

void Maratis::start(void)
{
	// gl version
	int GLversion = 0;
	const char * version = (const char *)glGetString(GL_VERSION);
	if(version)
    {
		sscanf(version, "%d", &GLversion);
	}

	// MEngine
	{
		MEngine * engine = MEngine::getInstance();

		// package manager
		engine->setPackageManager(m_packageManager);
		m_packageManager->init();

		// contexts
		engine->setSoundContext(m_soundContext); // sound context
		engine->setRenderingContext(m_render); // rendering context
		engine->setPhysicsContext(m_physics); // physics context
		engine->setScriptContext(m_script); // script context
		engine->setInputContext(m_input); // input context
		engine->setSystemContext(m_system); // system context

		// loaders
		engine->getImageLoader()->addLoader(M_loadImage); // image loader
		engine->getSoundLoader()->addLoader(M_loadSound); // sound loader
		engine->getLevelLoader()->addLoader(xmlLevelLoad); // level loader
		engine->getFontLoader()->addLoader(M_loadFont); // font loader
		engine->getFontLoader()->addLoader(M_loadBinFont); // bin font loader

		// add behaviors
		engine->getBehaviorManager()->addBehavior(MBLookAt::getStaticName(), M_OBJECT3D, MBLookAt::getNew);
		engine->getBehaviorManager()->addBehavior(MBFollow::getStaticName(), M_OBJECT3D, MBFollow::getNew);

		// add renderers
		if(GLversion >= 2)
			engine->getRendererManager()->addRenderer(MStandardRenderer::getStaticName(), MStandardRenderer::getNew);
		engine->getRendererManager()->addRenderer(MFixedRenderer::getStaticName(), MFixedRenderer::getNew);

		// mesh loaders
		engine->getMeshLoader()->addLoader(xmlMeshLoad);
		engine->getMeshLoader()->addLoader(M_loadBinMesh);
		engine->getMeshLoader()->addLoader(M_loadAssimpMesh);
		engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
		engine->getArmatureAnimLoader()->addLoader(M_loadBinArmatureAnim);
		engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
		engine->getTexturesAnimLoader()->addLoader(M_loadBinTexturesAnim);
		engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);
        engine->getMaterialsAnimLoader()->addLoader(M_loadBinMaterialsAnim);

		// level
		engine->setLevel(m_level);
		MScene * scene = m_level->addNewScene();
		scene->setName("Scene-1");

		// game
		engine->setGame(m_game);

		// set default renderer
		if(m_renderer == NULL)
		{
			if(GLversion >= 2)
				m_renderer = new MStandardRenderer();
			else
				m_renderer = new MFixedRenderer();
		}
		engine->setRenderer(m_renderer);
	}

	// view entities
	m_sphereEntity = new MOEntity	(loadEditorMesh("gui/meshs/sphere.mesh"));
	m_coneEntity = new MOEntity		(loadEditorMesh("gui/meshs/cone.mesh"));
	m_cubeEntity = new MOEntity		(loadEditorMesh("gui/meshs/box.mesh"));
	m_planeEntity = new MOEntity	(loadEditorMesh("gui/meshs/plane.mesh"));
	m_xEntity = new MOEntity		(loadEditorMesh("gui/meshs/x.mesh"));
	m_yEntity = new MOEntity		(loadEditorMesh("gui/meshs/y.mesh"));
	m_zEntity = new MOEntity		(loadEditorMesh("gui/meshs/z.mesh"));
	m_xcircleEntity = new MOEntity	(loadEditorMesh("gui/meshs/xcircle.mesh"));
	m_ycircleEntity = new MOEntity	(loadEditorMesh("gui/meshs/ycircle.mesh"));
	m_zcircleEntity = new MOEntity	(loadEditorMesh("gui/meshs/zcircle.mesh"));
	m_lightEntity = new MOEntity	(loadEditorMesh("gui/meshs/light.mesh"));
	m_cameraEntity = new MOEntity	(loadEditorMesh("gui/meshs/camera.mesh"));
	m_soundEntity = new MOEntity	(loadEditorMesh("gui/meshs/sound.mesh"));
}

void Maratis::clear(void)
{
	MEngine * engine = MEngine::getInstance();

	// view entities
	SAFE_DELETE(m_sphereEntity);
	SAFE_DELETE(m_coneEntity);
	SAFE_DELETE(m_cubeEntity);
	SAFE_DELETE(m_planeEntity);
	SAFE_DELETE(m_xEntity);
	SAFE_DELETE(m_yEntity);
	SAFE_DELETE(m_zEntity);
	SAFE_DELETE(m_xcircleEntity);
	SAFE_DELETE(m_ycircleEntity);
	SAFE_DELETE(m_zcircleEntity);
	SAFE_DELETE(m_lightEntity);
	SAFE_DELETE(m_cameraEntity);
	SAFE_DELETE(m_soundEntity);

	// level
	m_level->clear();
	if(m_renderer)
    {
		m_renderer->destroy();
		m_renderer = NULL;
	}

	// plugins
	SAFE_DELETE(m_gamePlugin);

	unsigned int i;
	unsigned int pSize = m_plugins.size();
	for(i=0; i<pSize; i++)
		SAFE_DELETE(m_plugins[i]);

	m_plugins.clear();

	// mesh manager
	m_meshManager.clear();

	// selected objects
	m_selectedObjects.clear();

	// MEngine
	{
		// loaders
		engine->getImageLoader()->clear();
		engine->getSoundLoader()->clear();
		engine->getLevelLoader()->clear();
		engine->getFontLoader()->clear();
		engine->getFontLoader()->clear();

		// behaviors
		engine->getBehaviorManager()->clear();

		// renderer
		engine->getRendererManager()->clear();

		// mesh loader
		engine->getMeshLoader()->clear();
		engine->getArmatureAnimLoader()->clear();
		engine->getTexturesAnimLoader()->clear();
		engine->getMaterialsAnimLoader()->clear();
	}
}

void Maratis::restart(void)
{
	clear();
	start();
}

void Maratis::initVue(void)
{
	// user vue
	m_perspectiveVue.setClippingNear(1);
	m_perspectiveVue.setClippingFar(100000);
	m_perspectiveVue.setPosition(MVector3(0, -200, 200));
	m_perspectiveVue.setEulerRotation(MVector3(40, 0, 0));
	m_perspectiveVue.updateMatrix();

	m_selectionCenter = MVector3(0, 0, 0);
	m_viewCenter = m_selectionCenter;
	clearSelectedObjects();
}

void Maratis::loadGamePlugin(void)
{
	MWindow * window = MWindow::getInstance();

	char gameFile[256];

	#ifdef WIN32
		getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.dll");
	#elif __APPLE__
		getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.dylib");
	#elif linux
		getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.so");
	#endif

	SAFE_DELETE(m_gamePlugin);
	m_gamePlugin = new MPlugin();
	m_gamePlugin->load(gameFile);
}

void Maratis::getNewObjectName(const char * objectName, char * name)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	unsigned int count = 0;
	int size = scene->getObjectsNumber();
	for(int i=0; i<size; i++)
	{
		MObject3d * object = scene->getObjectByIndex(i);
		if(object->getName())
            if(strcmp(name, object->getName()) == 0)
            {
                // name already exist
                count++;
                sprintf(name, "%s%d", objectName, count);
                i = -1;
            }
	}
}

void Maratis::duplicateSelectedObjects(void)
{
	autoSave();

	MaratisUI * UI = MaratisUI::getInstance();
	MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

	// get current objects number
	unsigned int objStart = scene->getObjectsNumber();
	unsigned int i;
	unsigned int oSize = m_selectedObjects.size();
	if(oSize == 0)
		return;

	// duplicate
	MOLight * light;
	MOEntity * entity;
	MOCamera * camera;
	MOSound * sound;
	MOText * text;
	MObject3d * object;
	for(i=0; i<oSize; i++)
	{
		object = m_selectedObjects[i];
		switch(object->getType())
		{
            case M_OBJECT3D_ENTITY:
			{
				char name[256] = "Entity0";
				getNewObjectName("Entity", name);

				entity = scene->addNewEntity(*(MOEntity *)object);
				entity->setName(name);
			}
                break;
            case M_OBJECT3D_LIGHT:
			{
				char name[256] = "Light0";
				getNewObjectName("Light", name);

				light = scene->addNewLight(*(MOLight *)object);
				light->setName(name);
			}
                break;
            case M_OBJECT3D_CAMERA:
			{
				char name[256] = "Camera0";
				getNewObjectName("Camera", name);

				camera = scene->addNewCamera(*(MOCamera *)object);
				camera->setName(name);
			}
                break;
            case M_OBJECT3D_SOUND:
			{
				char name[256] = "Sound0";
				getNewObjectName("Sound", name);

				sound = scene->addNewSound(*(MOSound *)object);
				sound->setName(name);
			}
                break;
            case M_OBJECT3D_TEXT:
			{
				char name[256] = "Text0";
				getNewObjectName("Text", name);

				text = scene->addNewText(*(MOText *)object);
				text->setName(name);
			}
                break;
		}
	}

	// select duplicated objects
	m_selectedObjects.clear();
	oSize = scene->getObjectsNumber();
	for(i=objStart; i<oSize; i++)
	{
		object = scene->getObjectByIndex(i);
		m_selectedObjects.push_back(object);
	}

	UI->editObject(object);
	UI->updateViewMenu();
}

void Maratis::linkSelectedObjects(void)
{
	autoSave();

	unsigned int i;
	unsigned int oSize = m_selectedObjects.size();
	if(oSize < 2)
		return;

	MObject3d * parent = m_selectedObjects[0];
	for(i=1; i<oSize; i++)
		linkTwoObjects(parent, m_selectedObjects[i]);
}

void Maratis::unlinkSelectedObjects(void)
{
	autoSave();

	unsigned int i;
	unsigned int oSize = m_selectedObjects.size();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = m_selectedObjects[i];
		unlinkTwoObjects(object->getParent(), object);
	}
}

void Maratis::linkTwoObjects(MObject3d *parent, MObject3d *child)
{
	if(parent == NULL || child == NULL)
		return;

	child->linkTo(parent);

	// local matrix
	MMatrix4x4 localMatrix = parent->getMatrix()->getInverse() * (*child->getMatrix());

	child->setPosition(localMatrix.getTranslationPart());
	child->setEulerRotation(localMatrix.getEulerAngles());

	float xSize = localMatrix.getRotatedVector3(MVector3(1, 0, 0)).getLength();
	float ySize = localMatrix.getRotatedVector3(MVector3(0, 1, 0)).getLength();
	float zSize = localMatrix.getRotatedVector3(MVector3(0, 0, 1)).getLength();

	child->setScale(MVector3(xSize, ySize, zSize));
}

void Maratis::unlinkTwoObjects(MObject3d *parent, MObject3d *child)
{
	if(parent == NULL || child == NULL)
		return;

	child->unLink();

	// matrix
	MMatrix4x4 * matrix = child->getMatrix();

	child->setPosition(matrix->getTranslationPart());
	child->setEulerRotation(matrix->getEulerAngles());

	float xSize = matrix->getRotatedVector3(MVector3(1, 0, 0)).getLength();
	float ySize = matrix->getRotatedVector3(MVector3(0, 1, 0)).getLength();
	float zSize = matrix->getRotatedVector3(MVector3(0, 0, 1)).getLength();

	child->setScale(MVector3(xSize, ySize, zSize));
}

void Maratis::deleteSelectedObjects(void)
{
	autoSave();

	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	unsigned int i;
	unsigned int oSize = m_selectedObjects.size();
	for(i=0; i<oSize; i++)
		scene->deleteObject(m_selectedObjects[i]);

	m_selectedObjects.clear();
}

void Maratis::addCamera(void)
{
	autoSave();

	char name[256] = "Camera0";
	getNewObjectName("Camera", name);

	MaratisUI * UI = MaratisUI::getInstance();
	MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

	MOCamera * camera = scene->addNewCamera();
	camera->setName(name);

	camera->setPosition(*getSelectionCenter());
	camera->setEulerRotation(MVector3(90, 0, 0));
	camera->updateMatrix();

	clearSelectedObjects();
	addSelectedObject(camera);
	UI->setTransformMode(M_TRANSFORM_POSITION);
	UI->editObject(camera);
	UI->updateViewMenu();
}

void Maratis::addLight(void)
{
	autoSave();

	char name[256] = "Light0";
	getNewObjectName("Light", name);

	MaratisUI * UI = MaratisUI::getInstance();
	MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

	MOLight * light = scene->addNewLight();
	light->setName(name);

	light->setRadius(1000);
	light->setPosition(*getSelectionCenter());
	light->updateMatrix();

	clearSelectedObjects();
	addSelectedObject(light);
	UI->setTransformMode(M_TRANSFORM_POSITION);
	UI->editObject(light);
}

void Maratis::okAddEntity(const char * filename)
{
	if(filename)
	{
		Maratis * maratis = Maratis::getInstance();
		MaratisUI * UI = MaratisUI::getInstance();
		MLevel * level = MEngine::getInstance()->getLevel();
		MScene * scene = level->getCurrentScene();

		MMeshRef * meshRef = level->loadMesh(filename);
		if(meshRef)
		{
			char name[256] = "Entity0";
			maratis->getNewObjectName("Entity", name);

			// add entity
			MOEntity * entity = scene->addNewEntity(meshRef);
			entity->setName(name);

			// set entity position
			entity->setPosition(*maratis->getSelectionCenter());
			entity->updateMatrix();

			maratis->clearSelectedObjects();
			maratis->addSelectedObject(entity);
			UI->setTransformMode(M_TRANSFORM_POSITION);
			UI->editObject(entity);
		}
	}
}

void Maratis::okAddSound(const char * filename)
{
	if(filename)
	{
		Maratis * maratis = Maratis::getInstance();
		MaratisUI * UI = MaratisUI::getInstance();
		MLevel * level = MEngine::getInstance()->getLevel();
		MScene * scene = level->getCurrentScene();

		MSoundRef * soundRef = level->loadSound(filename);
		if(soundRef)
		{
			char name[256] = "Sound0";
			maratis->getNewObjectName("Sound", name);

			// add sound
			MOSound * sound = scene->addNewSound(soundRef);
			sound->setName(name);

			// set position
			sound->setPosition(*maratis->getSelectionCenter());
			sound->updateMatrix();

			maratis->clearSelectedObjects();
			maratis->addSelectedObject(sound);
			UI->setTransformMode(M_TRANSFORM_POSITION);
			UI->editObject(sound);
		}
	}
}

void Maratis::okAddFont(const char * filename)
{
	if(filename)
	{
		Maratis * maratis = Maratis::getInstance();
		MaratisUI * UI = MaratisUI::getInstance();
		MLevel * level = MEngine::getInstance()->getLevel();
		MScene * scene = level->getCurrentScene();

		// font test
		MFontRef * fontRef = level->loadFont(filename);
		if(fontRef)
		{
			MOText * text = scene->addNewText(fontRef);

			char name[256] = "Text0";
			maratis->getNewObjectName("Text", name);

			text->setName(name);
			text->setText("Text");

			// set position
			text->setPosition(*maratis->getSelectionCenter());
			text->setEulerRotation(MVector3(180, 0, 0));
			text->updateMatrix();

			maratis->clearSelectedObjects();
			maratis->addSelectedObject(text);
			UI->setTransformMode(M_TRANSFORM_POSITION);
			UI->editObject(text);
		}
	}
}

void Maratis::addEntity(void)
{
	autoSave();

	MWindow * window = MWindow::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	char startPath[256];
	getGlobalFilename(startPath, window->getWorkingDirectory(), "meshs");
	UI->openFileBrowser(startPath, "", "load mesh", okAddEntity);
}

void Maratis::addSound(void)
{
	autoSave();

	MWindow * window = MWindow::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	char startPath[256];
	getGlobalFilename(startPath, window->getWorkingDirectory(), "sounds");
	UI->openFileBrowser(startPath, "", "load sound", okAddSound);
}

void Maratis::addText(void)
{
	autoSave();

	MWindow * window = MWindow::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	char startPath[256];
	getGlobalFilename(startPath, window->getWorkingDirectory(), "fonts");
    UI->openFileBrowser(startPath, "", "load font", okAddFont);
}

void Maratis::updateTitle(const char * additional)
{
	MWindow * window = MWindow::getInstance();

	char levelDir[256];
	char levelName[256];
	char title[256];

	getRepertory(levelDir, m_currentLevel);
	getLocalFilename(levelName, levelDir, m_currentLevel);

	if(additional)
		sprintf(title, "Maratis - %s - %s%s", m_currentProject, levelName, additional);
	else
		sprintf(title, "Maratis - %s - %s", m_currentProject, levelName);
	window->setTitle(title);
}

void Maratis::okNewProject(const char * filename)
{
	MWindow * window = MWindow::getInstance();
	Maratis * maratis = Maratis::getInstance();


	char file[256];
	fileExtension(file, filename, ".mproj");
	MProject proj;
	if(proj.saveXML(file))
	{
		// create empty dir
		char rep[256];
		char dir[256];
		getRepertory(rep, file);

		getGlobalFilename(dir, rep, "levels");
		createDirectory(dir);
		getGlobalFilename(dir, rep, "maps");
		createDirectory(dir);
		getGlobalFilename(dir, rep, "meshs");
		createDirectory(dir);
		getGlobalFilename(dir, rep, "sounds");
		createDirectory(dir);
		getGlobalFilename(dir, rep, "scripts");
		createDirectory(dir);
		getGlobalFilename(dir, rep, "shaders");
		createDirectory(dir);
		getGlobalFilename(dir, rep, "fonts");
		createDirectory(dir);

		// update
		window->setWorkingDirectory(rep);
		strcpy(maratis->m_currentProject, file);
		strcpy(maratis->m_currentLevel, "");
		maratis->updateTitle();

		// new
		maratis->restart();
		maratis->newLevel();
		maratis->loadGamePlugin();
	}
}

void Maratis::importExternal(void)
{
	MaratisUI * UI = MaratisUI::getInstance();

	if(strcmp(m_currentProject, "") != 0)
	{
		UI->openFileBrowser(NULL, "", "import", okImportExternal); // .dae, .obj, .x, .xml(ogre), .md3...
	}
	else
	{
		UI->setPopupButton("you need to open a project to import external models", NULL);
	}
}

void Maratis::okImportExternal(const char * filename)
{
	M_importAssimpMeshes(filename);
}

void Maratis::newProject(void)
{
	MaratisUI * UI = MaratisUI::getInstance();
	UI->openFileBrowser(NULL, "untitled.mproj", "save project", okNewProject);
}

void Maratis::okLoadProject(const char * filename)
{
	Maratis * maratis = Maratis::getInstance();
	maratis->loadProject(filename);
}

void Maratis::loadProject(void)
{
	MaratisUI * UI = MaratisUI::getInstance();
	UI->openFileBrowser(NULL, "", "open project", okLoadProject);
}

void Maratis::loadProject(const char * filename)
{
	MWindow * window = MWindow::getInstance();

	if(! filename)
		return;

	// load project file
	MProject proj;
	if(proj.loadXML(filename))
	{
		// update
		char workingDir[256];
		getRepertory(workingDir, filename);
		window->setWorkingDirectory(workingDir);
		strcpy(m_currentProject, filename);

		// game plugin
		restart();
		loadGamePlugin();

		// renderer
		changeRenderer(proj.renderer.c_str());


		// test pack
		// if we have a package manager, try to load the package
		if(MPackageManager* pPackMan = MEngine::getInstance()->getPackageManager())
		{
			char projName[256];
			getLocalFilename(projName, workingDir, filename);
			if(char* ext = strstr(projName, ".mproj"))
			{
				sprintf(ext, ".npk");
				char packageFile[256];
				getGlobalFilename(packageFile, workingDir, projName);
				pPackMan->loadPackage(packageFile);
			}
		}


		// load start level
		if(! loadLevel(proj.startLevel.c_str()))
		{
			newLevel();
		}
	}
}

void Maratis::newLevel(void)
{
	MEngine * engine = MEngine::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	// init maratis
	initVue();

	// clear level
	MLevel * level = engine->getLevel();
	level->clear();

	// add first scene
	MScene * scene = level->addNewScene();
	scene->setName("Scene-1");

	// update scene name edit
	MGuiEditText * sceneEdit = UI->getSceneEdit();
	sceneEdit->setText(scene->getName());

	// clear windows
	UI->editObject(NULL);

	// update scene menu
	UI->updateSceneMenu();
	UI->updateViewMenu();

	strcpy(m_currentLevel, "");
	updateTitle();
}

bool Maratis::loadLevel(const char * filename)
{
	MaratisUI * UI = MaratisUI::getInstance();
	MEngine * engine = MEngine::getInstance();

	if(! filename)
		return false;

	// load level
	MLevel * level = engine->getLevel();
	if(engine->getLevelLoader()->loadData(filename, level))
	{
		strcpy(m_currentLevel, filename);

		// init maratis
		initVue();

		// clear windows
		UI->editObject(NULL);

		// update scene menu
		UI->updateSceneMenu();
		UI->updateViewMenu();

		// update matrices
		level->getCurrentScene()->updateObjectsMatrices();

		clearUndo();
		updateTitle();
		return true;
	}

	return false;
}

void Maratis::okLoadLevel(const char * filename)
{
	Maratis * maratis = Maratis::getInstance();
	maratis->loadLevel(filename);
}

void Maratis::loadLevel(void)
{
	MWindow * window = MWindow::getInstance();

	char startPath[256];
	getGlobalFilename(startPath, window->getWorkingDirectory(), "levels");

	MaratisUI * UI = MaratisUI::getInstance();
	UI->openFileBrowser(startPath, "", "open level", okLoadLevel);
}

void Maratis::save(void)
{
	MEngine * engine = MEngine::getInstance();

	if(m_currentLevel[0] == '\0')
    {
		saveAs();
	}
	else
	{
		xmlLevelSave(engine->getLevel(), m_currentLevel);
		MProject proj;
		proj.startLevel = m_currentLevel;
		if(m_renderer)
		{
			const char * name = m_renderer->getName();
			if(name)
				proj.renderer = name;
		}
		if(! proj.saveXML(m_currentProject))
			newProject();
	}

	updateTitle();
}

void Maratis::okSaveAs(const char * filename)
{
	if(! filename)
		return;

	Maratis * maratis = Maratis::getInstance();
	MEngine * engine = MEngine::getInstance();

	// save level
	char file[256];
	fileExtension(file, filename, ".level");
	if(xmlLevelSave(engine->getLevel(), file))
	{
		strcpy(maratis->m_currentLevel, file);
		MProject proj;
		proj.startLevel = maratis->m_currentLevel;
		if(maratis->m_renderer)
		{
			const char * name = maratis->m_renderer->getName();
			if(name)
				proj.renderer = name;
		}
		proj.saveXML(maratis->m_currentProject);
		maratis->updateTitle();
	}
}

void Maratis::saveAs(void)
{
	MWindow * window = MWindow::getInstance();

	char startPath[256];
	getGlobalFilename(startPath, window->getWorkingDirectory(), "levels");

	MaratisUI * UI = MaratisUI::getInstance();
	UI->openFileBrowser(startPath, "untitled.level", "save level", okSaveAs);
}

void Maratis::addSelectedObject(MObject3d * object)
{
	unsigned int i;
	unsigned int oSize = m_selectedObjects.size();

	for(i=0; i<oSize; i++)
	{
		if(m_selectedObjects[i] == object)
		{
			m_selectedObjects.erase(m_selectedObjects.begin() + i);
			return;
		}
	}

	m_selectedObjects.push_back(object);
}

void Maratis::rotateCurrentVue(void)
{
	MMouse * mouse = MMouse::getInstance();

	// inverse center
	MVector3 lCenter = m_perspectiveVue.getInversePosition(m_viewCenter);

	// rotation
	m_perspectiveVue.addAxisAngleRotation(MVector3(1, 0, 0), -(float)mouse->getYDirection()*0.65f);

	MMatrix4x4 matrix;
	MQuaternion rotation = m_perspectiveVue.getRotation();
	matrix.setRotationAxis(rotation.getAngle(), rotation.getAxis());
	matrix.invert();

	MVector3 axis = matrix * MVector3(0, 0, 1);
	m_perspectiveVue.addAxisAngleRotation(axis, -(float)mouse->getXDirection()*0.65f);
	m_perspectiveVue.updateMatrix();

	// position
	MVector3 fCenter = m_perspectiveVue.getTransformedVector(lCenter);
	m_perspectiveVue.setPosition(m_perspectiveVue.getPosition() + (m_viewCenter - fCenter));

	m_perspectiveVue.updateMatrix();
}

void Maratis::panCurrentVue(void)
{
	MMouse * mouse = MMouse::getInstance();
	MOCamera * camera = getPerspectiveVue();

	int * viewport = camera->getCurrentViewport();

	MVector3 vecX(1, 0, 0);
	MVector3 vecZ(0, 1, 0);
	MVector3 xAxis = camera->getRotatedVector(vecX);
	MVector3 zAxis = camera->getRotatedVector(vecZ);

	if(camera->isOrtho())
	{
		// position
		MVector3 position = camera->getPosition();
		float dFactor = camera->getFov() / (viewport[3]);

		camera->setPosition(position + ((xAxis * (-(float)mouse->getXDirection())) + (zAxis * (float)mouse->getYDirection()))*dFactor);
		camera->updateMatrix();

		return;
	}

	// position
	MVector3 position = camera->getPosition();
	MVector3 axis = camera->getRotatedVector(MVector3(0, 0, -1));

	float z = (m_viewCenter - position).dotProduct(axis);
	float fovFactor = camera->getFov() * 0.0192f;

	float dx = - ((mouse->getXDirection() / (float)viewport[3]) * z) * fovFactor;
	float dy = ((mouse->getYDirection() / (float)viewport[3]) * z) * fovFactor;

	camera->setPosition(position + (xAxis * dx) + (zAxis * dy));
	camera->updateMatrix();
}

void Maratis::zoomCurrentVue(void)
{
	MMouse * mouse = MMouse::getInstance();
	MOCamera * camera = getPerspectiveVue();
	MVector3 position = camera->getPosition();

	if(camera->isOrtho())
	{
		int dir = mouse->getWheelDirection();
		camera->setFov(camera->getFov() - (camera->getFov() * dir * 0.15f));

		MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;

		float factor = (camera->getFov() - 1);
		float dist = (m_viewCenter - position).dotProduct(cameraAxis);

		camera->setPosition(position + (cameraAxis * (dist - factor)));
		camera->updateMatrix();

		return;
	}

	// dir
	float dir = (-mouse->getWheelDirection()) * 20.0f;
	float dFactor = (m_viewCenter - position).getLength() * 0.01f;

	MVector3 vec(0, 0, -1);
	MVector3 axis = camera->getRotatedVector(vec);

	camera->setPosition(position - (axis * dir * dFactor));
	camera->updateMatrix();
}

void Maratis::switchCurrentVueMode(void)
{
	MOCamera * camera = getPerspectiveVue();
	MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;
	MVector3 position = camera->getPosition();

	float dist = (m_viewCenter - position).dotProduct(cameraAxis);

	if(camera->isOrtho())
	{
		float factor = (camera->getFov() - 1);

		camera->setPosition(position + (cameraAxis * (dist - factor)));
		camera->updateMatrix();

		camera->setFov(60);
		camera->setClippingNear(1);
		camera->setClippingFar(100000);
		camera->enableOrtho(false);
		return;
	}

	camera->setClippingNear(-1000);
	camera->setClippingFar(50000);
	camera->setFov(1 + dist);
	camera->enableOrtho(true);
}

void Maratis::changeCurrentVue(int vue)
{
	MOCamera * camera = getPerspectiveVue();
	MVector3 position = camera->getPosition();

	// set to ortho
	if(! camera->isOrtho())
		switchCurrentVueMode();

	float dist = (m_viewCenter - position).getLength();

	// set vue
	switch(vue)
	{
        case 1:
            camera->setPosition(MVector3(0, -dist, 0));
            camera->setEulerRotation(MVector3(90, 0, 0));
            break;
        case 3:
            camera->setPosition(MVector3(dist, 0, 0));
            camera->setEulerRotation(MVector3(90, 0, 90));
            break;
        case 7:
            camera->setPosition(MVector3(0, 0, dist));
            camera->setEulerRotation(MVector3(0, 0, 0));
            break;
        case 9:
            camera->setPosition(MVector3(0, 0, -dist));
            camera->setEulerRotation(MVector3(180, 0, 0));
            break;
	}

	camera->updateMatrix();
}

void Maratis::drawBoundingBox(MBox3d * box)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	MVector3 * min = &box->min;
	MVector3 * max = &box->max;

	beginDraw(M_PRIMITIVE_LINES);

	pushVertex(MVector3(min->x, min->y, min->z));
	pushVertex(MVector3(max->x, min->y, min->z));

	pushVertex(MVector3(min->x, min->y, min->z));
	pushVertex(MVector3(min->x, max->y, min->z));

	pushVertex(MVector3(min->x, min->y, min->z));
	pushVertex(MVector3(min->x, min->y, max->z));

	pushVertex(MVector3(max->x, max->y, max->z));
	pushVertex(MVector3(min->x, max->y, max->z));

	pushVertex(MVector3(max->x, max->y, max->z));
	pushVertex(MVector3(max->x, min->y, max->z));

	pushVertex(MVector3(max->x, max->y, max->z));
	pushVertex(MVector3(max->x, max->y, min->z));

	pushVertex(MVector3(max->x, max->y, min->z));
	pushVertex(MVector3(min->x, max->y, min->z));

	pushVertex(MVector3(max->x, min->y, max->z));
	pushVertex(MVector3(min->x, min->y, max->z));

	pushVertex(MVector3(max->x, max->y, min->z));
	pushVertex(MVector3(max->x, min->y, min->z));

	pushVertex(MVector3(min->x, max->y, max->z));
	pushVertex(MVector3(min->x, min->y, max->z));

	pushVertex(MVector3(min->x, max->y, max->z));
	pushVertex(MVector3(min->x, max->y, min->z));

	pushVertex(MVector3(max->x, min->y, max->z));
	pushVertex(MVector3(max->x, min->y, min->z));

	endDraw(render);
}

bool getNearestRaytracedDistance(MMesh * mesh, MMatrix4x4 * matrix, const MVector3 & origin, const MVector3 & dest, float * distance, MOEntity * entity = NULL)
{
	// animate armature
	if(entity && mesh->getArmature())
	{
		MArmature * armature = mesh->getArmature();
		if(mesh->getArmatureAnim())
		{
			animateArmature(
				mesh->getArmature(),
				mesh->getArmatureAnim(),
				entity->getCurrentFrame()
			);
		}
		else
		{
			armature->processBonesLinking();
			armature->updateBonesSkinMatrix();
		}
	}

	MMatrix4x4 iMatrix = matrix->getInverse();

	MVector3 localOrigin = iMatrix * origin;
	MVector3 localDest = iMatrix * dest;

	MBox3d * box = mesh->getBoundingBox();
	if(! isEdgeToBoxCollision(localOrigin, localDest, box->min, box->max))
		return false;

	bool raytraced = false;

	float dist;
	float nearDist;
	MVector3 I, intersectionPoint;
	MVector3 rayVector = localDest - localOrigin;

	// init near dist
	nearDist =  rayVector.getSquaredLength();

	unsigned int s;
	unsigned int sSize = mesh->getSubMeshsNumber();
	for(s=0; s<sSize; s++)
	{
		MSubMesh * subMesh = &mesh->getSubMeshs()[s];
		box = subMesh->getBoundingBox();

		// skinning
		MVector3 * vertices = subMesh->getVertices();
		if(entity && mesh->getArmature() && subMesh->getSkinData())
		{
			unsigned int verticesSize = subMesh->getVerticesSize();
			MVector3 * skinVertices = MSkinCache::getInstance()->getVertices(verticesSize);
			computeSkinning(mesh->getArmature(), subMesh->getSkinData(), vertices, NULL, NULL, skinVertices, NULL, NULL);
			vertices = skinVertices;
		}

		if(isEdgeToBoxCollision(localOrigin, localDest, box->min, box->max))
		{
			unsigned int d;
			unsigned int dSize = subMesh->getDisplaysNumber();
			for(d=0; d<dSize; d++)
			{
				MDisplay * display = subMesh->getDisplay(d);

				// indices
				void * indices = subMesh->getIndices();
				switch(subMesh->getIndicesType())
				{
					case M_USHORT:
					{
						unsigned short * idx = (unsigned short *)indices;
						indices = (void *)(idx + display->getBegin());
						break;
					}
					case M_UINT:
					{
						unsigned int * idx = (unsigned int *)indices;
						indices = (void *)(idx + display->getBegin());
						break;
					}

                    default:
                        break;
				}

				// BACK or FRONT and BACK, scan ray
				if((display->getCullMode() == M_CULL_BACK) || (display->getCullMode() == M_CULL_NONE))
				{

					if(getNearestRaytracedPosition(
												   localOrigin, localDest,
												   indices,
												   subMesh->getIndicesType(),
												   vertices,
												   display->getSize(),
												   &I))
					{
						dist = (I - localOrigin).getSquaredLength();
						if(dist < nearDist)
						{
							intersectionPoint = I;
							nearDist = dist;
							raytraced = true;
						}
					}
				}

				// FRONT or FRONT and BACK, scan invert
				if((display->getCullMode() == M_CULL_FRONT) || (display->getCullMode() == M_CULL_NONE))
				{
					if(getNearestRaytracedPosition(
												   localOrigin, localDest,
												   indices,
												   subMesh->getIndicesType(),
												   vertices,
												   display->getSize(),
												   &I, 1))
					{
						dist = (I - localOrigin).getSquaredLength();
						if(dist < nearDist)
						{
							intersectionPoint = I;
							nearDist = dist;
							raytraced = true;
						}
					}
				}
			}
		}
	}

	if(raytraced)
		*distance = (((*matrix) * intersectionPoint) - origin).getLength();

	return raytraced;
}

MObject3d * Maratis::getNearestMesh(MScene * scene, const MVector3 & rayO, const MVector3 & rayD, MVector3 * intersectPoint)
{
	float distance = 0;
	MObject3d * nearestObject = NULL;
	
	
	unsigned int i;
	unsigned int oSize = scene->getEntitiesNumber();
	for(i=0; i<oSize; i++)
	{
		MOEntity * entity = scene->getEntityByIndex(i);
		
		if(! entity->isActive())
			continue;
		
		if(entity->isInvisible())
			continue;

		float dist;
		
		MMatrix4x4 * matrix = entity->getMatrix();
		
		MMesh * mesh = entity->getMesh();
		if(mesh)
		{
			if(! getNearestRaytracedDistance(mesh, matrix, rayO, rayD, &dist, entity))
				continue;
		}
		else{
			continue;
		}
	
		if(! nearestObject)
		{
			nearestObject = entity;
			distance = dist;
			continue;
		}
		
		if(dist > distance)
			continue;
		
		nearestObject = entity;
		distance = dist;
	}
	
	if(intersectPoint && nearestObject)
	{
		*intersectPoint = rayO + (rayD - rayO).getNormalized()*distance;
	}
	
	return nearestObject;
}

MObject3d * Maratis::getNearestObject(MScene * scene, const MVector3 & rayO, const MVector3 & rayD, MVector3 * intersectPoint)
{
	// get camera
	MOCamera * camera = getPerspectiveVue();

	int * viewport = camera->getCurrentViewport();
	float editObjectSize = 10.0f / (float)viewport[3];

	// scan objects
	float distance = 0;
	MObject3d * nearestObject = NULL;

	unsigned int i;
	unsigned int oSize = scene->getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = scene->getObjectByIndex(i);

		//if(! object->isActive())
		//	continue;

		float dist;
		switch(object->getType())
		{
            case M_OBJECT3D_ENTITY:
			{
				MOEntity * entity = (MOEntity *)object;
				MMatrix4x4 * matrix = entity->getMatrix();

				MMesh * mesh = entity->getMesh();
				if(mesh)
				{
					if(! getNearestRaytracedDistance(mesh, matrix, rayO, rayD, &dist, entity))
						continue;
				}
				else{
					continue;
				}
			}
                break;
            case M_OBJECT3D_CAMERA:
			{
				float scale;

				if(! camera->isOrtho())
					scale = (object->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
				else
					scale = camera->getFov() * editObjectSize;

				if(scale < 0.001f)
					continue;

				MMatrix4x4 scaleMatrix;
				scaleMatrix.setScale(MVector3(scale, scale, scale));
				MMatrix4x4 matrix = ((*object->getMatrix()) * scaleMatrix);

				MMesh * mesh = m_cameraEntity->getMesh();
				if(! getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
					continue;
			}
                break;
            case M_OBJECT3D_LIGHT:
			{
				float scale;

				if(! camera->isOrtho())
					scale = (object->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
				else
					scale = camera->getFov() * editObjectSize;

				if(scale < 0.001f)
					continue;

				MMatrix4x4 scaleMatrix;
				scaleMatrix.setScale(MVector3(scale, scale, scale));
				MMatrix4x4 matrix = ((*object->getMatrix()) * scaleMatrix);

				MMesh * mesh = m_sphereEntity->getMesh();
				if(! getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
					continue;
			}
                break;
            case M_OBJECT3D_SOUND:
			{
				float scale;

				if(! camera->isOrtho())
					scale = (object->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
				else
					scale = camera->getFov() * editObjectSize;

				if(scale < 0.001f)
					continue;

				MMatrix4x4 scaleMatrix;
				scaleMatrix.setScale(MVector3(scale, scale, scale)*1.75f);
				MMatrix4x4 matrix = ((*object->getMatrix()) * scaleMatrix);

				MMesh * mesh = m_sphereEntity->getMesh();
				if(! getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
					continue;
			}
                break;
            case M_OBJECT3D_TEXT:
			{
				MOText * text = (MOText*)object;
				MVector3 min = text->getBoundingBox()->min;
				MVector3 max = text->getBoundingBox()->max;

				if((min.x == 0 && max.x == 0 && min.y == 0 && max.y == 0) || (! text->isActive()))
				{
					m_emptyText.setFontRef(text->getFontRef());
					min = m_emptyText.getBoundingBox()->min;
					max = m_emptyText.getBoundingBox()->max;
				}

				MMatrix4x4 scaleMatrix;
				scaleMatrix.setScale(MVector3(max.x - min.x, max.y - min.y, 1));
				scaleMatrix.setTranslationPart(min);

				MMatrix4x4 matrix = (*object->getMatrix()) * scaleMatrix;

				MMesh * mesh = m_planeEntity->getMesh();
				if(! getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
					continue;
			}
                break;
            default:
                continue;
		}

		if(! nearestObject)
		{
			nearestObject = object;
			distance = dist;
			continue;
		}

		if(dist > distance)
			continue;

		nearestObject = object;
		distance = dist;
	}

	
	if(intersectPoint && nearestObject)
	{
		*intersectPoint = rayO + (rayD - rayO).getNormalized()*distance;
	}
	
	return nearestObject;
}

void Maratis::selectAll(void)
{
	autoSave();

	MaratisUI * UI = MaratisUI::getInstance();
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	bool allSelect = (m_selectedObjects.size() == scene->getObjectsNumber());

	clearSelectedObjects();

	if(! allSelect)
	{
		unsigned int i;
		unsigned int size = scene->getObjectsNumber();
		for(i=0; i<size; i++)
			addSelectedObject(scene->getObjectByIndex(i));
	}
	else
	{
		UI->editObject(NULL);
	}
}

void Maratis::focusSelection(void)
{
	if(m_selectedObjects.empty())
		return;

	float distance = 0.0f;
	MVector3 center(0, 0, 0);
	for(std::vector <MObject3d *>::iterator itObj = m_selectedObjects.begin(); itObj != m_selectedObjects.end(); ++itObj)
	{
		MObject3d *pobj = *itObj;
		center += pobj->getTransformedPosition();
		switch(pobj->getType())
		{
			case M_OBJECT3D:
			case M_OBJECT3D_BONE:
			case M_OBJECT3D_CAMERA:
			case M_OBJECT3D_LIGHT:
			case M_OBJECT3D_SOUND:
			case M_OBJECT3D_TEXT:
				distance = 20.0f;
				break;

			case M_OBJECT3D_ENTITY:
				{
					MOEntity *pentity = (MOEntity*)pobj;
					MBox3d *pbox = pentity->getBoundingBox();

					MVector3 scale = pentity->getTransformedScale();
					float maxScale = scale.x;
					maxScale = MAX(maxScale, scale.y);
					maxScale = MAX(maxScale, scale.z);

					float radius = (pbox->min - pbox->max).getLength() * 1.5f * maxScale;
					if(radius > distance)
						distance = radius;
				}
				break;
		}
	}

	center /= (float)m_selectedObjects.size();

	MOCamera * camera = getPerspectiveVue();
	MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();
	camera->setPosition(center - cameraAxis * distance);
	camera->updateMatrix();
	
	m_viewCenter = m_selectionCenter;
}

void Maratis::activeSelection(void)
{
	autoSave();

	unsigned int i;
	unsigned int selSize = getSelectedObjectsNumber();
	for(i=0; i<selSize; i++)
	{
		MObject3d * object = getSelectedObjectByIndex(i);
		object->setActive(! object->isActive());
	}
}

void Maratis::selectSameMesh(void)
{
	autoSave();

	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	std::vector <MMesh *> meshs;

	unsigned int i;
	unsigned int selSize = getSelectedObjectsNumber();
	for(i=0; i<selSize; i++)
	{
		MObject3d * object = getSelectedObjectByIndex(i);
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity *)object;
			MMesh * mesh = entity->getMesh();
			if(mesh)
				meshs.push_back(mesh);
		}
	}

	unsigned int meshsSize = meshs.size();
	if(meshsSize == 0)
		return;

	clearSelectedObjects();

	unsigned int j;
	unsigned int size = scene->getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		MOEntity * entity = scene->getEntityByIndex(i);
		MMesh * mesh = entity->getMesh();
		for(j=0; j<meshsSize; j++){
			if(meshs[j] == mesh){
				addSelectedObject(entity);
				break;
			}
		}
	}
}

void Maratis::selectObjectsInMainView(MScene * scene)
{
	// get window
	MWindow * window = MWindow::getInstance();
	MaratisUI * UI = MaratisUI::getInstance();

	// is mouse over
	if((! UI->getView()->isMouseInside()) || (! window->isMouseOverWindow()))
		return;

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	// get camera
	MOCamera * camera = getPerspectiveVue();
	MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;

	// viewport
	int * viewport = camera->getCurrentViewport();

	// ray (origin and dest)
	MVector3 rayO = camera->getTransformedPosition();
	MVector3 rayD = camera->getUnProjectedPoint(MVector3(
                                                         (float)mouse->getXPosition(),
                                                         (float)(window->getHeight() - mouse->getYPosition()),
                                                         1));

	if(camera->isOrtho())
	{
		MVector3 point;
		isRayPlaneIntersection(rayD, -cameraAxis, rayO + (cameraAxis * camera->getClippingNear()), cameraAxis, &point);
		rayO = point;
	}

	rayD = rayO + ((rayD - rayO).getNormalized() * (camera->getClippingFar() - camera->getClippingNear()));

	// transform
	if((! UI->isShortcutEngaged(&UI->m_scMultipleSelection, false)) && (getSelectedObjectsNumber() > 0) && (getTransformMode() != M_TRANSFORM_MOUSE))
	{
		float radius;
		MVector3 position;

		if(! camera->isOrtho())
		{
			radius = (camera->getFov() * 400) / (float)viewport[3];
			MVector3 cameraDir = (*getSelectionCenter()) - rayO;
			cameraDir.normalize();
			position = rayO + cameraDir*200;
		}
		else
		{
			radius = (camera->getFov() * 120) / (float)viewport[3];
			MVector3 point;
			isRayPlaneIntersection((*getSelectionCenter()), -cameraAxis, rayO, cameraAxis, &point);
			position = point + cameraAxis*((camera->getClippingFar() - camera->getClippingNear()) * 0.5f);
		}

		switch(getTransformMode())
		{
            case M_TRANSFORM_ROTATION:
                m_currentAxis = selectEditRotation(camera, rayO, rayD, position, radius);
                break;

            case M_TRANSFORM_POSITION:
                m_currentAxis = selectEditPosition(camera, rayO, rayD, position, radius);
                break;

            case M_TRANSFORM_SCALE:
                m_currentAxis = selectEditScale(camera, rayO, rayD, position, radius);
                break;

            default:
                break;
		}

		if(m_currentAxis != M_AXIS_NONE)
			return;
	}

	// clear selection
	if(! UI->isShortcutEngaged(&UI->m_scMultipleSelection, false))
		clearSelectedObjects();

	MObject3d * nearestObject = getNearestObject(scene, rayO, rayD);

	if(nearestObject)
	{
		autoSave();
		addSelectedObject(nearestObject);
	}

	// edit object
	unsigned int oSize = getSelectedObjectsNumber();
	if(oSize == 0)
	{
		UI->editObject(NULL);
		return;
	}

	UI->editObject(getSelectedObjectByIndex(oSize - 1));
}

void Maratis::updateViewCenter(void)
{
	// view center
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	
	MVector3 rayO = m_perspectiveVue.getTransformedPosition();
	MVector3 rayD = m_perspectiveVue.getTransformedVector(MVector3(0, 0, -m_perspectiveVue.getClippingFar()));
	
	getNearestMesh(scene, rayO, rayD, &m_viewCenter);
}

void Maratis::updateSelectionCenter(void)
{	
	MVector3 position(0, 0, 0);

	unsigned int i;
	unsigned int oSize = getSelectedObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getSelectedObjectByIndex(i);
		position += object->getTransformedPosition();
	}

	if(oSize > 0)
		m_selectionCenter = position / (float)oSize;
}

void Maratis::drawGrid(MScene * scene)
{
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	render->disableFog();
	render->enableDepthTest();
	render->disableTexture();
	render->disableBlending();

	// grid
	unsigned int i;
	unsigned int gridSpace = 10;
	unsigned int gridNumber = 4;
	unsigned int gSize = gridNumber*10;
	float gLength = (float)(gSize*gridSpace);

	// colors
	MVector3 clearColor = MVector3(0.2f, 0.3f, 0.4f);
	MVector3 color1 = (clearColor * 0.8f);
	MVector3 color2 = (clearColor * 0.9f);
	MVector3 red =   (clearColor * 0.7f) + (MVector3(1, 0, 0) * 0.3f);
	MVector3 green = (clearColor * 0.7f) + (MVector3(0, 1, 0) * 0.3f);

	// draw lines
	render->setColor3(color2);
	beginDraw(M_PRIMITIVE_LINES);
	for(i=0; i<=gSize; i++)
	{
		float length = (float)(i*gridSpace);

		pushVertex(MVector3(-length, -gLength, 0));
		pushVertex(MVector3(-length,  gLength, 0));

		pushVertex(MVector3( length, -gLength, 0));
		pushVertex(MVector3( length,  gLength, 0));

		pushVertex(MVector3(-gLength, -length, 0));
		pushVertex(MVector3( gLength, -length, 0));

		pushVertex(MVector3(-gLength, length, 0));
		pushVertex(MVector3( gLength, length, 0));
	}
	endDraw(render);

	render->disableDepthTest();

	render->setColor3(color1);
	beginDraw(M_PRIMITIVE_LINES);
	for(i=0; i<=gSize; i+=gridSpace)
	{
		float length = (float)(i*gridSpace);

		pushVertex(MVector3(-length, -gLength, 0));
		pushVertex(MVector3(-length,  gLength, 0));

		pushVertex(MVector3( length, -gLength, 0));
		pushVertex(MVector3( length,  gLength, 0));

		pushVertex(MVector3(-gLength, -length, 0));
		pushVertex(MVector3( gLength, -length, 0));

		pushVertex(MVector3(-gLength, length, 0));
		pushVertex(MVector3( gLength, length, 0));
	}
	endDraw(render);

	// axis
	render->setColor3(red);
	beginDraw(M_PRIMITIVE_LINES);
	pushVertex(MVector3(-gLength, 0, 0));
	pushVertex(MVector3( gLength, 0, 0));
	endDraw(render);

	render->setColor3(green);
	beginDraw(M_PRIMITIVE_LINES);
	pushVertex(MVector3(0, -gLength, 0));
	pushVertex(MVector3(0,  gLength, 0));
	endDraw(render);
}

void Maratis::drawScaleAxis(M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, const bool viewTest)
{
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	MVector3 position = matrix->getTranslationPart();

	// normal
	MVector3 normal;
	MObject3d * object = NULL;

	switch(axis)
	{
        case M_AXIS_X:
            normal = MVector3(1, 0, 0);
            object = m_xEntity;
            break;

        case M_AXIS_Y:
            normal = MVector3(0, 1, 0);
            object = m_yEntity;
            break;

        case M_AXIS_Z:
            normal = MVector3(0, 0, 1);
            object = m_zEntity;
            break;

        default:break;
	}

	// camera direction
	MVector3 cameraPos = camera->getTransformedPosition();
	MVector3 cameraDir;

	if(! camera->isOrtho())
	{
		cameraDir = position - cameraPos;
		cameraDir.normalize();
	}
	else
	{
		cameraDir = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;
	}

	// view test
	if(viewTest)
	{
		MVector3 axisNormal = matrix->getRotatedVector3(normal).getNormalized();

		// view factor
		float viewFactor = fabs(cameraDir.dotProduct(axisNormal));

		bool visible = (viewFactor < 0.98f);
		if(object)
			object->setVisible(visible);

		if(! visible)
			return;
	}

	// draw
	render->pushMatrix();
	render->multMatrix(matrix);

	switch(axis)
	{
        case M_AXIS_X:
            render->rotate(MVector3(0, 1, 0), 90);
            break;

        case M_AXIS_Y:
            render->rotate(MVector3(1, 0, 0), -90);
            break;

        default:
            break;
	}

	beginDraw(M_PRIMITIVE_LINES);
	pushVertex(MVector3(0, 0, 0.3f));
	pushVertex(MVector3(0, 0, 0.9f));
	endDraw(render);

	render->translate(MVector3(0, 0, 0.9f));
	drawTriangles(m_cubeEntity->getMesh());

	render->popMatrix();
}

void Maratis::drawPositionAxis(M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, const bool viewTest)
{
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// get position from matrix
	MVector3 position = matrix->getTranslationPart();

	// normal
	MVector3 normal;
	MObject3d * object = NULL;

	switch(axis)
	{
        case M_AXIS_X:
            normal = MVector3(1, 0, 0);
            object = m_xEntity;
            break;

        case M_AXIS_Y:
            normal = MVector3(0, 1, 0);
            object = m_yEntity;
            break;

        case M_AXIS_Z:
            normal = MVector3(0, 0, 1);
            object = m_zEntity;
            break;

        default:
            break;
	}

	// camera direction
	MVector3 cameraPos = camera->getTransformedPosition();
	MVector3 cameraDir;

	if(! camera->isOrtho())
	{
		cameraDir = position - cameraPos;
		cameraDir.normalize();
	}
	else
	{
		cameraDir = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;
	}

	// view test
	if(viewTest)
	{
		MVector3 axisNormal = matrix->getRotatedVector3(normal).getNormalized();

		// view factor
		float viewFactor = fabs(cameraDir.dotProduct(axisNormal));

		bool visible = (viewFactor < 0.98f);
		if(object)
			object->setVisible(visible);

		if(! visible)
			return;
	}

	// draw
	render->pushMatrix();
	render->multMatrix(matrix);

	switch(axis)
	{
        case M_AXIS_X:
            render->rotate(MVector3(0, 1, 0), 90);
            break;

        case M_AXIS_Y:
            render->rotate(MVector3(1, 0, 0), -90);
            break;

        default:
            break;
	}

	beginDraw(M_PRIMITIVE_LINES);
	pushVertex(MVector3(0, 0, 0.3f));
	pushVertex(MVector3(0, 0, 0.9f));
	endDraw(render);

	render->translate(MVector3(0, 0, 0.9f));
	drawTriangles(m_coneEntity->getMesh());

	render->popMatrix();
}

void Maratis::drawRotationCircle(M_AXIS axis, MOCamera * camera, MMatrix4x4 * matrix, const bool zTest)
{
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// normal
	MVector3 normal;
	MVector3 vector;

	MVector3 position = matrix->getTranslationPart();

	// camera direction
	MVector3 cameraPos = camera->getTransformedPosition();
	MVector3 cameraDir;

	if(! camera->isOrtho())
	{
		cameraDir = position - cameraPos;
		cameraDir.normalize();
	}
	else
	{
		cameraDir = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;
	}

	switch(axis)
	{
        case M_AXIS_X:
            normal = MVector3(1, 0, 0);
            vector = MVector3(0, 1, 0);
            break;

        case M_AXIS_Y:
            normal = MVector3(0, 1, 0);
            vector = MVector3(1, 0, 0);
            break;

        case M_AXIS_Z:
            normal = MVector3(0, 0, 1);
            vector = MVector3(1, 0, 0);
            break;

        default:
            break;
	}

	if(! zTest)
		render->disableDepthTest();
	else
	{
		MVector3 axisNormal = matrix->getRotatedVector3(normal).getNormalized();

		// view factor
		float viewFactor = fabs(cameraDir.dotProduct(axisNormal));

		// depth test
		if(viewFactor < 0.98f)
			render->enableDepthTest();
		else
			render->disableDepthTest();
	}

	render->pushMatrix();
	render->translate(- (cameraDir * 4));
	render->multMatrix(matrix);

	// draw
	beginDraw(M_PRIMITIVE_LINE_STRIP);

	MVector3 vec;
	unsigned int i;
	for(i=0; i<=360; i+=10)
	{
		vec = vector.getRotatedAxis((double)i, normal);
		pushVertex(vec);
	}

	endDraw(render);
	render->popMatrix();
}

void Maratis::computeTransformDirection(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, const float distance, const MVector3 & axis)
{
	// get window
	MWindow * window = MWindow::getInstance();

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	// intersection
	MVector3 intersection = rayO + ((rayD - rayO).getNormalized() * distance);

	// normal
	MVector3 normal = getTriangleNormal(position, intersection, position + axis);

	// mouse position
	m_tMousePosition = MVector2((float)mouse->getXPosition(), (float)mouse->getYPosition());

	// vector direction
	MVector3 pPoint = camera->getProjectedPoint(intersection - (normal * 100));
	pPoint.y = window->getHeight() - pPoint.y;

	m_tVectorDirection = (MVector2(pPoint.x, pPoint.y) - m_tMousePosition).getNormalized();
}

void Maratis::computeTransformPlane(MOCamera * camera, const MVector3 & position, const MVector3 & axis)
{
	// get window
	MWindow * window = MWindow::getInstance();

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	// plane
	MVector3 up = camera->getRotatedVector(MVector3(1, 1, 1));
	m_tPlane = getTriangleNormal(MVector3(0, 0, 0), axis, up);

	// center position
	MVector3 pPoint = camera->getProjectedPoint(position);
	pPoint.y = window->getHeight() - pPoint.y;

	m_tCenterPosition = MVector2(pPoint.x, pPoint.y);

	// vector direction
	pPoint = camera->getProjectedPoint(position + (axis * 100));
	pPoint.y = window->getHeight() - pPoint.y;

	m_tVectorDirection = MVector2(pPoint.x, pPoint.y) - m_tCenterPosition;
	m_tVectorDirection.normalize();

	// mouse position
	m_tMousePosition = MVector2((float)mouse->getXPosition(), (float)mouse->getYPosition());

	// offset direction
	m_tOffsetDirection = m_tMousePosition - m_tCenterPosition;
}

M_AXIS Maratis::selectEditScale(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius)
{
	// axis
	MVector3 radiusScale = MVector3(radius, radius, radius);
	MVector3 xAxis(1, 0, 0);
	MVector3 yAxis(0, 1, 0);
	MVector3 zAxis(0, 0, 1);

	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// rotation
	MVector3 rotation(0, 0, 0);
	if(oSize == 1)
	{
		MObject3d * object = getSelectedObjectByIndex(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		MMatrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

		rotation = matrix.getEulerAngles();

		xAxis = matrix.getRotatedVector3(xAxis);
		yAxis = matrix.getRotatedVector3(yAxis);
		zAxis = matrix.getRotatedVector3(zAxis);
	}

	// matrice
	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(radiusScale);

	// raytrace axis meshs
	float dist;
	float distance;
	M_AXIS axis = M_AXIS_NONE;

	// z
	if(m_zEntity->isVisible())
	{
		MMesh * mesh = m_zEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
		{
			axis = M_AXIS_Z;
			distance = dist;
			computeTransformPlane(camera, position, zAxis);
		}
	}

	// y
	if(m_yEntity->isVisible())
	{
		MMesh * mesh = m_yEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
		{
			if(axis == M_AXIS_NONE)
			{
				axis = M_AXIS_Y;
				distance = dist;
			}
			else if(dist < distance)
			{
				axis = M_AXIS_Y;
				distance = dist;
			}
			computeTransformPlane(camera, position, yAxis);
		}
	}

	// x
	if(m_xEntity->isVisible())
	{
		MMesh * mesh = m_xEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
		{
			if(axis == M_AXIS_NONE)
			{
				axis = M_AXIS_X;
				distance = dist;
			}
			else if(dist < distance)
			{
				axis = M_AXIS_X;
				distance = dist;
			}
			computeTransformPlane(camera, position, xAxis);
		}
	}

	// view axis
	if(axis == M_AXIS_NONE)
	{
		MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

		unsigned int i;
		for(i=0; i<oSize; i++)
		{
			MObject3d * object = getSelectedObjectByIndex(i);
			if(getNearestObject(scene, rayO, rayD) == object)
			{
				computeTransformPlane(camera, position, MVector3(0, 0, 0));
				autoSave();
				return M_AXIS_VIEW;
			}
		}
	}
	else{
		autoSave();
	}

	return axis;
}

M_AXIS Maratis::selectEditPosition(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, float radius)
{
	// axis
	MVector3 radiusScale = MVector3(radius, radius, radius);
	MVector3 xAxis(1, 0, 0);
	MVector3 yAxis(0, 1, 0);
	MVector3 zAxis(0, 0, 1);

	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// rotation
	MVector3 rotation(0, 0, 0);
	if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
	{
		MObject3d * object = getSelectedObjectByIndex(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		MMatrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

		rotation = matrix.getEulerAngles();

		xAxis = matrix.getRotatedVector3(xAxis);
		yAxis = matrix.getRotatedVector3(yAxis);
		zAxis = matrix.getRotatedVector3(zAxis);
	}

	// matrice
	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(radiusScale);

	// raytrace axis meshs
	float dist;
	float distance;
	M_AXIS axis = M_AXIS_NONE;

	// z
	if(m_zEntity->isVisible())
	{
		MMesh * mesh = m_zEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
		{
			axis = M_AXIS_Z;
			distance = dist;
			computeTransformPlane(camera, position, zAxis);
		}
	}

	// y
	if(m_yEntity->isVisible())
	{
		MMesh * mesh = m_yEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
		{
			if(axis == M_AXIS_NONE)
			{
				axis = M_AXIS_Y;
				distance = dist;
			}
			else if(dist < distance)
			{
				axis = M_AXIS_Y;
				distance = dist;
			}
			computeTransformPlane(camera, position, yAxis);
		}
	}

	// x
	if(m_xEntity->isVisible())
	{
		MMesh * mesh = m_xEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
		{
			if(axis == M_AXIS_NONE)
			{
				axis = M_AXIS_X;
				distance = dist;
			}
			else if(dist < distance)
			{
				axis = M_AXIS_X;
				distance = dist;
			}
			computeTransformPlane(camera, position, xAxis);
		}
	}

	// view axis
	if(axis == M_AXIS_NONE)
	{
		MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();
		MObject3d * nearestObj = getNearestObject(scene, rayO, rayD);

		unsigned int i;
		for(i=0; i<oSize; i++)
		{
			MObject3d * object = getSelectedObjectByIndex(i);
			if(object == nearestObj)
			{
				computeTransformPlane(camera, position, MVector3(0, 0, 0));
				autoSave();
				return M_AXIS_VIEW;
			}
		}
	}
	else{
		autoSave();
	}

	return axis;
}

M_AXIS Maratis::selectEditRotation(MOCamera * camera, const MVector3 & rayO, const MVector3 & rayD, const MVector3 & position, const float radius)
{
	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	MVector3 radiusScale = MVector3(radius, radius, radius);
	MVector3 xAxis(1, 0, 0);
	MVector3 yAxis(0, 1, 0);
	MVector3 zAxis(0, 0, 1);

	// rotation
	MVector3 rotation(0, 0, 0);
	if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
	{
		MObject3d * object = getSelectedObjectByIndex(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		MMatrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

		rotation = matrix.getEulerAngles();

		xAxis = matrix.getRotatedVector3(xAxis);
		yAxis = matrix.getRotatedVector3(yAxis);
		zAxis = matrix.getRotatedVector3(zAxis);
	}

	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(radiusScale);

	// raytrace cirle meshs
	float distance;
	float cDist = (position - rayO).getLength() * 1.1f;

	// z
	{
		MMesh * mesh = m_zcircleEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &distance))
			if(distance < cDist)
			{
				computeTransformDirection(camera, rayO, rayD, position, distance, zAxis);
				autoSave();
				return M_AXIS_Z;
			}
	}

	// y
	{
		MMesh * mesh = m_ycircleEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &distance))
			if(distance < cDist)
			{
				computeTransformDirection(camera, rayO, rayD, position, distance, yAxis);
				autoSave();
				return M_AXIS_Y;
			}
	}

	// x
	{
		MMesh * mesh = m_xcircleEntity->getMesh();
		if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &distance))
			if(distance < cDist)
			{
				computeTransformDirection(camera, rayO, rayD, position, distance, xAxis);
				autoSave();
				return M_AXIS_X;
			}
	}

	// view axis
	MScene * scene = MEngine::getInstance()->getLevel()->getCurrentScene();

	unsigned int i;
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getSelectedObjectByIndex(i);
		if(getNearestObject(scene, rayO, rayD) == object)
		{
			computeTransformPlane(camera, position, MVector3(0, 0, 0));
			autoSave();
			return M_AXIS_VIEW;
		}
	}

	return M_AXIS_NONE;
}

void Maratis::drawEditPosition(MOCamera * camera)
{
	// viewport
	int * viewport = camera->getCurrentViewport();

	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	float radius;
	MVector3 position;

	if(! camera->isOrtho())
	{
		radius = (camera->getFov() * 400) / (float)viewport[3];
		MVector3 cameraPos = camera->getTransformedPosition();
		MVector3 cameraDir = (*getSelectionCenter()) - cameraPos;
		cameraDir.normalize();
		position = cameraPos + cameraDir*200;
	}
	else
	{
		radius = (camera->getFov() * 120) / (float)viewport[3];
		MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;
		MVector3 point;
		isRayPlaneIntersection((*getSelectionCenter()), -cameraAxis, camera->getTransformedPosition(), cameraAxis, &point);
		position = point + cameraAxis*200;
	}

	MVector3 radiusScale = MVector3(radius, radius, radius);

	// rotation
	MVector3 rotation(0, 0, 0);
	if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
	{
		MObject3d * object = getSelectedObjectByIndex(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		rotation = ((*object->getMatrix()) * iScaleMatrix).getEulerAngles();
	}

	// matrix
	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(radiusScale);

	// axis
	if(mouse->isLeftButtonPushed() && (m_currentAxis != M_AXIS_NONE))
	{
		switch(m_currentAxis)
		{
            case M_AXIS_X:
                render->setColor3(MVector3(1, 1, 0));
                drawPositionAxis(M_AXIS_X, camera, &matrix, false);
                return;

            case M_AXIS_Y:
                render->setColor3(MVector3(1, 1, 0));
                drawPositionAxis(M_AXIS_Y, camera, &matrix, false);
                return;

            case M_AXIS_Z:
                render->setColor3(MVector3(1, 1, 0));
                drawPositionAxis(M_AXIS_Z, camera, &matrix, false);
                return;

            case M_AXIS_VIEW:
                return;

            default:
                break;
		}
	}

	// draw axis
	render->setColor3(MVector3(1, 0, 0));
	drawPositionAxis(M_AXIS_X, camera, &matrix);
	render->setColor3(MVector3(0, 1, 0));
	drawPositionAxis(M_AXIS_Y, camera, &matrix);
	render->setColor3(MVector3(0, 0, 1));
	drawPositionAxis(M_AXIS_Z, camera, &matrix);
}

void Maratis::drawEditScale(MOCamera * camera)
{
	// viewport
	int * viewport = camera->getCurrentViewport();

	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	float radius;
	MVector3 position;

	if(! camera->isOrtho())
	{
		radius = (camera->getFov() * 400) / (float)viewport[3];
		MVector3 cameraPos = camera->getTransformedPosition();
		MVector3 cameraDir = (*getSelectionCenter()) - cameraPos;
		cameraDir.normalize();
		position = cameraPos + cameraDir*200;
	}
	else
	{
		radius = (camera->getFov() * 120) / (float)viewport[3];
		MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;
		MVector3 point;
		isRayPlaneIntersection((*getSelectionCenter()), -cameraAxis, camera->getTransformedPosition(), cameraAxis, &point);
		position = point + cameraAxis*200;
	}

	MVector3 radiusScale = MVector3(radius, radius, radius);

	// rotation
	MVector3 rotation(0, 0, 0);
	if(oSize == 1)
	{
		MObject3d * object = getSelectedObjectByIndex(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		rotation = ((*object->getMatrix()) * iScaleMatrix).getEulerAngles();
	}

	// matrix
	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(radiusScale);

	// axis
	if(mouse->isLeftButtonPushed() && (m_currentAxis != M_AXIS_NONE))
	{
		switch(m_currentAxis)
		{
            case M_AXIS_X:
                render->setColor3(MVector3(1, 1, 0));
                drawScaleAxis(M_AXIS_X, camera, &matrix, false);
                return;

            case M_AXIS_Y:
                render->setColor3(MVector3(1, 1, 0));
                drawScaleAxis(M_AXIS_Y, camera, &matrix, false);
                return;

            case M_AXIS_Z:
                render->setColor3(MVector3(1, 1, 0));
                drawScaleAxis(M_AXIS_Z, camera, &matrix, false);
                return;

            case M_AXIS_VIEW:
                return;

            default:
                break;
		}
	}

	// draw axis
	render->setColor3(MVector3(1, 0, 0));
	drawScaleAxis(M_AXIS_X, camera, &matrix);
	render->setColor3(MVector3(0, 1, 0));
	drawScaleAxis(M_AXIS_Y, camera, &matrix);
	render->setColor3(MVector3(0, 0, 1));
	drawScaleAxis(M_AXIS_Z, camera, &matrix);
}

void Maratis::drawEditRotation(MOCamera * camera)
{
	// viewport
	int * viewport = camera->getCurrentViewport();

	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// get engine
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	float radius;
	MVector3 position;
	MVector3 cameraPos = camera->getTransformedPosition();
	MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;

	if(! camera->isOrtho())
	{
		radius = (camera->getFov() * 400) / (float)viewport[3];
		MVector3 cameraDir = (*getSelectionCenter()) - cameraPos;
		cameraDir.normalize();
		position = cameraPos + cameraDir*200;
	}
	else
	{
		radius = (camera->getFov() * 120) / (float)viewport[3];
		MVector3 point;
		isRayPlaneIntersection((*getSelectionCenter()), -cameraAxis, camera->getTransformedPosition(), cameraAxis, &point);
		position = point + cameraAxis*200;
	}

	MVector3 radiusScale = MVector3(radius, radius, radius);

	// rotation
	MVector3 rotation(0, 0, 0);
	if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
	{
		MObject3d * object = getSelectedObjectByIndex(0);
		MVector3 worldScale = object->getTransformedScale();

		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

		rotation = ((*object->getMatrix()) * iScaleMatrix).getEulerAngles();
	}

	// matrix
	MMatrix4x4 matrix;
	matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
	matrix.setTranslationPart(position);
	matrix.scale(radiusScale);

	// draw sphere mask
	render->pushMatrix();
	render->multMatrix(&matrix);
	render->setColorMask(0, 0, 0, 0);
	drawTriangles(m_sphereEntity->getMesh());
	render->setColorMask(1, 1, 1, 1);
	render->popMatrix();

	// enable blending
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);

	// draw view circle
	render->setColor4(MVector4(1, 1, 1, 0.2f));

	MMatrix4x4 eyeMatrix;
	if(! camera->isOrtho())
	{
		MQuaternion quat;
		quat.setFromVectors(MVector3(0, 0, 1), (*getSelectionCenter()) - cameraPos);
		eyeMatrix.setRotationAxis(quat.getAngle(), quat.getAxis());
	}
	else
	{
		MQuaternion rotation = camera->getRotation();
		eyeMatrix.setRotationAxis(rotation.getAngle(), rotation.getAxis());
	}

	eyeMatrix.setTranslationPart(position);
	eyeMatrix.scale(radiusScale);
	drawRotationCircle(M_AXIS_Z, camera, &eyeMatrix, false);


	// draw circles
	if(mouse->isLeftButtonPushed() && (m_currentAxis != M_AXIS_NONE))
	{
		switch(m_currentAxis)
		{
            case M_AXIS_X:
                render->setColor4(MVector4(1, 1, 1, 0.2f));
                drawRotationCircle(M_AXIS_X, camera, &matrix, false);
                render->setColor3(MVector3(1, 1, 0));
                drawRotationCircle(M_AXIS_X, camera, &matrix);
                return;

            case M_AXIS_Y:
                render->setColor4(MVector4(1, 1, 1, 0.2f));
                drawRotationCircle(M_AXIS_Y, camera, &matrix, false);
                render->setColor3(MVector3(1, 1, 0));
                drawRotationCircle(M_AXIS_Y, camera, &matrix);
                return;

            case M_AXIS_Z:
                render->setColor4(MVector4(1, 1, 1, 0.2f));
                drawRotationCircle(M_AXIS_Z, camera, &matrix, false);
                render->setColor3(MVector3(1, 1, 0));
                drawRotationCircle(M_AXIS_Z, camera, &matrix);
                return;

            case M_AXIS_VIEW:
                return;

            default:
                break;
		}
	}

	render->setColor3(MVector3(1, 0, 0));
	drawRotationCircle(M_AXIS_X, camera, &matrix);
	render->setColor3(MVector3(0, 1, 0));
	drawRotationCircle(M_AXIS_Y, camera, &matrix);
	render->setColor3(MVector3(0, 0, 1));
	drawRotationCircle(M_AXIS_Z, camera, &matrix);
}

void Maratis::transformRotation(void)
{
	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// get mouse
	MMouse * mouse = MMouse::getInstance();
	MVector2 dir = MVector2((float)mouse->getXDirection(), (float)mouse->getYDirection());

	// angle
	float angle;
	switch(m_currentAxis)
	{
        case M_AXIS_X:
        case M_AXIS_Y:
        case M_AXIS_Z:
            angle = m_tVectorDirection.dotProduct(dir);
            break;
        case M_AXIS_VIEW:
		{
			MVector2 vec = (m_tMousePosition - m_tCenterPosition).getNormalized();
			angle = MVector2(-vec.y, vec.x).dotProduct(dir);
		}
            break;

        default:
            break;
	}

	// get camera
	MOCamera * camera = getPerspectiveVue();

	// rotate
	if(getOrientationMode() == M_ORIENTATION_WORLD)
	{
		MVector3 axis;
		MVector3 * position = getSelectionCenter();

		switch(m_currentAxis)
		{
            case M_AXIS_X:
                axis = MVector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = MVector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = MVector3(0, 0, 1);
                break;

            case M_AXIS_VIEW:
			{
				if(camera->isOrtho())
					axis = camera->getRotatedVector(MVector3(0, 0, -1));
				else
					axis = (*getSelectionCenter() - camera->getTransformedPosition()).getNormalized();
			}
                break;

            default:
                break;
		}

		unsigned int i;
		for(i=0; i<oSize; i++)
		{
			MObject3d * object = getSelectedObjectByIndex(i);

			if(object->hasParent())
			{
				MObject3d * parent = object->getParent();
				if(isObjectSelected(parent))
					continue;

				// translate
				MVector3 vec = object->getTransformedPosition() - (*position);
				vec.rotateAxis((double)angle, axis);

				MVector3 localPos = parent->getInversePosition((*position) + vec);
				object->setPosition(localPos);

				// rotate
				MVector3 iAxis = object->getInverseRotatedVector(axis) / object->getTransformedScale();

				object->addAxisAngleRotation(iAxis, angle);
			}
			else
			{
				// translate
				MVector3 vec = object->getTransformedPosition() - (*position);
				vec.rotateAxis((double)angle, axis);
				object->setPosition((*position) + vec);

				// rotate
				MVector3 iAxis = object->getInverseRotatedVector(axis) / object->getTransformedScale();
				object->addAxisAngleRotation(iAxis, angle);
			}

			// update matrix
			object->updateMatrix();
		}
	}
	else
	{
		MVector3 axis;

		switch(m_currentAxis)
		{
            case M_AXIS_X:
                axis = MVector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = MVector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = MVector3(0, 0, 1);
                break;

            case M_AXIS_VIEW:
			{
				if(camera->isOrtho())
					axis = camera->getRotatedVector(MVector3(0, 0, -1));
			}
                break;

            default:
                break;
		}

		unsigned int i;
		for(i=0; i<oSize; i++)
		{
			MObject3d * object = getSelectedObjectByIndex(i);

			if(m_currentAxis == M_AXIS_VIEW)
			{
				if(! camera->isOrtho())
					axis = (object->getTransformedPosition() - camera->getTransformedPosition()).getNormalized();

				MVector3 iAxis = object->getInverseRotatedVector(axis) / object->getTransformedScale();
				object->addAxisAngleRotation(iAxis, angle);
				object->updateMatrix();
				continue;
			}

			object->addAxisAngleRotation(axis, angle);
			object->updateMatrix();
		}
	}
}

void Maratis::transformScale(void)
{
	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	// position
	MVector3 * position = getSelectionCenter();

	// mouse direction
	MVector2 mouseDir = MVector2((float)mouse->getXDirection(), (float)mouse->getYDirection());

	// view axis
	if(m_currentAxis == M_AXIS_VIEW)
	{
		// scale factor
		MVector2 dir = (m_tMousePosition - m_tCenterPosition).getNormalized();
		float scaleFactor = 1.0f + dir.dotProduct(mouseDir) * 0.01f;

		unsigned int i;
		for(i=0; i<oSize; i++)
		{
			MObject3d * object = getSelectedObjectByIndex(i);

			if(getOrientationMode() == M_ORIENTATION_WORLD)
			{
				if(object->hasParent())
				{
					MObject3d * parent = object->getParent();
					if(isObjectSelected(parent))
						continue;

					MVector3 worldPos = object->getTransformedPosition();
					MVector3 localPos = parent->getInversePosition((*position) + ((worldPos - (*position)) * scaleFactor));
					object->setPosition(localPos);
				}
				else
				{
					MVector3 worldPos = object->getTransformedPosition();
					object->setPosition((*position) + ((worldPos - (*position)) * scaleFactor));
				}
			}

			MVector3 scale = object->getScale() * scaleFactor;

			object->setScale(scale);
			object->updateMatrix();
		}

		return;
	}

	// axis
	MVector3 axis;
	if(oSize > 1)
	{
		switch(m_currentAxis)
		{
            case M_AXIS_X:
                axis = MVector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = MVector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = MVector3(0, 0, 1);
                break;

            default:
                break;
		}
	}
	else
	{
		MObject3d * object = getSelectedObjectByIndex(0);

		// matrix
		MVector3 scale = object->getTransformedScale();
		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z));
		MMatrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

		switch(m_currentAxis)
		{
            case M_AXIS_X:
                axis = matrix.getRotatedVector3(MVector3(1, 0, 0));
                break;

            case M_AXIS_Y:
                axis = matrix.getRotatedVector3(MVector3(0, 1, 0));
                break;

            case M_AXIS_Z:
                axis = matrix.getRotatedVector3(MVector3(0, 0, 1));
                break;

            default:
                break;
		}
	}

	// scale factor
	float scaleFactor = 1.0f + m_tVectorDirection.dotProduct(mouseDir) * 0.01f;

	unsigned int i;
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getSelectedObjectByIndex(i);
		MVector3 worldPos = object->getTransformedPosition();

		MVector3 scale = object->getScale();
		switch(m_currentAxis)
		{
            case M_AXIS_X:
                scale.x *= scaleFactor;
                break;
            case M_AXIS_Y:
                scale.y *= scaleFactor;
                break;
            case M_AXIS_Z:
                scale.z *= scaleFactor;
                break;

            default:
                break;
		}

		object->setScale(scale);
		object->updateMatrix();
	}
}

void Maratis::transformPosition(void)
{
	// objects
	unsigned int oSize = getSelectedObjectsNumber();

	// get window
	MWindow * window = MWindow::getInstance();

	// get mouse
	MMouse * mouse = MMouse::getInstance();

	// get camera
	MOCamera * camera = getPerspectiveVue();

	// position
	updateSelectionCenter();
	MVector3 * position = getSelectionCenter();

	// mouse position
	MVector2 mousePos((float)mouse->getXPosition(), (float)mouse->getYPosition());
	mousePos = mousePos - m_tOffsetDirection;

	// view axis
	if(m_currentAxis == M_AXIS_VIEW)
	{
		MVector3 viewAxis = camera->getRotatedVector(MVector3(0, 0, -1));

		// ray (origin and dest)
		MVector3 rayO = camera->getTransformedPosition();
		MVector3 rayD = camera->getUnProjectedPoint(
                                                    MVector3((float)mousePos.x, (float)(window->getHeight() - mousePos.y), 1)
                                                    );

		if(camera->isOrtho())
		{
			MVector3 point;
			isRayPlaneIntersection(rayD, -viewAxis, rayO, viewAxis, &point);
			rayO = point;
			rayD = (rayD - rayO).getNormalized();
		}
		else
		{
			rayD = (rayD - rayO).getNormalized();
		}

		// intersection
		MVector3 point;
		isRayPlaneIntersection(rayO, rayD, (*position), viewAxis, &point);

		unsigned int i;
		for(i=0; i<oSize; i++)
		{
			MObject3d * object = getSelectedObjectByIndex(i);
			MVector3 worldPos = object->getTransformedPosition();

			if(object->hasParent())
			{
				MObject3d * parent = object->getParent();
				if(! isObjectSelected(parent))
				{
					MVector3 localPos = parent->getInversePosition(point + (worldPos - (*position)));
					object->setPosition(localPos);
				}
			}
			else
			{
				object->setPosition(point + (worldPos - (*position)));
			}
			object->updateMatrix();
		}

		return;
	}

	// axis
	MVector3 axis;
	if((getOrientationMode() == M_ORIENTATION_WORLD) || (oSize > 1))
	{
		switch(m_currentAxis)
		{
            case M_AXIS_X:
                axis = MVector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = MVector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = MVector3(0, 0, 1);
                break;

            default:
                break;
		}
	}
	else
	{
		MObject3d * object = getSelectedObjectByIndex(0);

		// matrix
		MVector3 scale = object->getTransformedScale();
		MMatrix4x4 iScaleMatrix;
		iScaleMatrix.setScale(MVector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z));
		MMatrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

		switch(m_currentAxis)
		{
            case M_AXIS_X:
                axis = matrix.getRotatedVector3(MVector3(1, 0, 0));
                break;

            case M_AXIS_Y:
                axis = matrix.getRotatedVector3(MVector3(0, 1, 0));
                break;

            case M_AXIS_Z:
                axis = matrix.getRotatedVector3(MVector3(0, 0, 1));
                break;

            default:
                break;
		}
	}

	// mouse position on axis
	MVector2 axisNormal(-m_tVectorDirection.y, m_tVectorDirection.x);
	float axisMouseDist = (mousePos - m_tCenterPosition).dotProduct(axisNormal);
	mousePos = mousePos - (axisNormal * axisMouseDist);

	MVector3 viewAxis = camera->getRotatedVector(MVector3(0, 0, -1));

	// ray (origin and dest)
	MVector3 rayO = camera->getTransformedPosition();
	MVector3 rayD = camera->getUnProjectedPoint(
                                                MVector3(mousePos.x, (window->getHeight() - mousePos.y), 0.499238f)
                                                );

	if(camera->isOrtho())
	{
		MVector3 point;
		isRayPlaneIntersection(rayD, -viewAxis, rayO, viewAxis, &point);
		rayO = point;
		rayD = (rayD - rayO).getNormalized();
	}
	else
	{
		rayD = (rayD - rayO).getNormalized();
	}

	// intersection
	MVector3 point;
	isRayPlaneIntersection(rayO, rayD, (*position), m_tPlane, &point);

	float distance = axis.dotProduct((point) - (*position));

	unsigned int i;
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = getSelectedObjectByIndex(i);
		MVector3 worldPos = object->getTransformedPosition();

		if(object->hasParent())
		{
			MObject3d * parent = object->getParent();
			if(! isObjectSelected(parent))
			{
				MVector3 localPos = parent->getInversePosition(worldPos + (axis * distance));
				object->setPosition(localPos);
			}
		}
		else
		{
			object->setPosition(worldPos + (axis * distance));
		}

		object->updateMatrix();
	}
}

void Maratis::transformSelectedObjects(void)
{
	if(getSelectedObjectsNumber() < 1)
		return;

	if(m_currentAxis == M_AXIS_NONE)
		return;

	switch(getTransformMode())
	{
        case M_TRANSFORM_ROTATION:
            transformRotation();
            break;

        case M_TRANSFORM_POSITION:
            transformPosition();
            break;

        case M_TRANSFORM_SCALE:
            transformScale();
            break;

        default:
            break;
	}
}

bool Maratis::isObjectSelected(MObject3d * object)
{
	unsigned int i;
	unsigned int oSize = getSelectedObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		if(getSelectedObjectByIndex(i) == object)
			return true;
	}

	return false;
}

void Maratis::drawInvisibleEntity(MOEntity * entity)
{
	// HACK opengl
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();
	MMesh * mesh = entity->getMesh();
	if(mesh)
	{
		render->pushMatrix();
		render->multMatrix(entity->getMatrix());

		// draw
		drawTriangles(mesh);

		render->popMatrix();
	}

	// HACK opengl
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Maratis::drawCamera(MScene * scene, MOCamera * camera)
{
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// clear z buffer
	render->enableDepthTest();
	render->clear(M_BUFFER_DEPTH);

	// draw scene
	camera->enable();
	scene->draw(camera);
	scene->drawObjectsBehaviors();
}

void Maratis::drawTriangles(MMesh * mesh)
{
	// get render
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// subMeshs
	unsigned int s;
	unsigned sSize = mesh->getSubMeshsNumber();
	for(s=0; s<sSize; s++)
	{
		MSubMesh * subMesh = &mesh->getSubMeshs()[s];

		render->disableLighting();
		render->disableNormalArray();
		render->disableColorArray();
		render->disableTexCoordArray();

		// vertices
		render->enableVertexArray();
		render->setVertexPointer(M_FLOAT, 3, subMesh->getVertices());

		unsigned int i;
		unsigned int displayNumber = subMesh->getDisplaysNumber();
		for(i=0; i<displayNumber; i++)
		{
			MDisplay * display = subMesh->getDisplay(i);
			if(! display->isVisible())
				continue;

			// begin / size
			unsigned int begin = display->getBegin();
			unsigned int size = display->getSize();

			// display properties
			M_PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
			M_CULL_MODES cullMode = display->getCullMode();

			// cull mode
			render->setCullMode(cullMode);

			// indices
			void * indices = subMesh->getIndices();

			// draw
			if(indices)
			{
				switch(subMesh->getIndicesType())
				{
                    case M_USHORT:
                        render->drawElement(primitiveType, size, M_USHORT, (unsigned short*)indices + begin);
                        break;

                    case M_UINT:
                        render->drawElement(primitiveType, size, M_UINT, (unsigned int*)indices + begin);
                        break;

                    default:
                        break;
				}
			}
			else
				render->drawArray(primitiveType, begin, size);
		}
	}
}

void Maratis::drawLight(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	MMesh * mesh = m_lightEntity->getMesh();
	unsigned short * indices = (unsigned short *)mesh->getSubMeshs()[0].getIndices();
	MVector3 * vertices = mesh->getSubMeshs()[0].getVertices();

	unsigned int i;
	for(i=0; i<mesh->getSubMeshs()[0].getIndicesSize(); i+=3)
	{
		beginDraw(M_PRIMITIVE_LINES);

		pushVertex(vertices[indices[i+0]]);
		pushVertex(vertices[indices[i+1]]);

		pushVertex(vertices[indices[i+1]]);
		pushVertex(vertices[indices[i+2]]);
		endDraw(render);
	}
}

void Maratis::drawCamera(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	MMesh * mesh = m_cameraEntity->getMesh();
	unsigned short * indices = (unsigned short *)mesh->getSubMeshs()[0].getIndices();
	MVector3 * vertices = mesh->getSubMeshs()[0].getVertices();

	unsigned int i;
	for(i=0; i<mesh->getSubMeshs()[0].getIndicesSize(); i+=3)
	{
		beginDraw(M_PRIMITIVE_LINES);

		pushVertex(vertices[indices[i+0]]);
		pushVertex(vertices[indices[i+1]]);

		pushVertex(vertices[indices[i+1]]);
		pushVertex(vertices[indices[i+2]]);
		endDraw(render);
	}
}

void Maratis::drawSound(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	MMesh * mesh = m_soundEntity->getMesh();
	unsigned short * indices = (unsigned short *)mesh->getSubMeshs()[0].getIndices();
	MVector3 * vertices = mesh->getSubMeshs()[0].getVertices();

	unsigned int i;
	for(i=0; i<mesh->getSubMeshs()[0].getIndicesSize(); i+=3)
	{
		beginDraw(M_PRIMITIVE_LINES);

		pushVertex(vertices[indices[i+0]]);
		pushVertex(vertices[indices[i+1]]);

		pushVertex(vertices[indices[i+1]]);
		pushVertex(vertices[indices[i+2]]);
		endDraw(render);
	}
}

void Maratis::drawArmature(MOEntity * entity)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	
	MMesh * mesh = entity->getMesh();
	if(mesh)
	{
		MArmature * armature = mesh->getArmature();
		if(armature)
		{
			if(mesh->getArmatureAnim())
			{
				animateArmature(
					mesh->getArmature(),
					mesh->getArmatureAnim(),
					entity->getCurrentFrame()
				);
			}
			else
			{
				armature->processBonesLinking();
				armature->updateBonesSkinMatrix();
			}
			
			MVector3 iVue = m_perspectiveVue.getPosition();
			
			beginDraw(M_PRIMITIVE_LINES);
			
			unsigned int b, bSize = armature->getBonesNumber();
			for(b=0; b<bSize; b++)
			{
				MOBone * bone = armature->getBone(b);
				
				MVector3 pos = entity->getTransformedVector(bone->getTransformedPosition());
				float size = (pos - iVue).getLength() * 0.01f;
				
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(-size, 0, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3( size, 0, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0, -size, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0,  size, 0)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0, 0, -size)));
				pushVertex(pos + bone->getUniformRotatedVector(MVector3(0, 0,  size)));
			}
			
			endDraw(render);
		}
	}
}

void Maratis::drawMainView(MScene * scene)
{
	// get mouse
	MMouse * mouse = MMouse::getInstance();

	MEngine * engine = MEngine::getInstance();
	
	// get render
	MRenderingContext * render = engine->getRenderingContext();
	MRenderer * renderer = engine->getRenderer();

	// get camera
	MOCamera * camera = getPerspectiveVue();
	MVector3 cameraAxis = camera->getRotatedVector(MVector3(0, 0, -1)).getNormalized();;

	// clear z buffer
	render->enableDepthTest();
	render->clear(M_BUFFER_DEPTH);

	// draw grid
	camera->enable();
	drawGrid(scene);

	render->enableDepthTest();
	scene->draw(camera);
	scene->drawObjectsBehaviors();

	// draw extra (box, triggers...)
	render->disableLighting();
	render->disableTexture();
	render->enableBlending();
	render->setBlendingMode(M_BLENDING_ALPHA);

	int * viewport = camera->getCurrentViewport();
	float editObjectSize = 10.0f / (float)viewport[3];

	// draw inactive / invisibles entities
	unsigned int i;
	unsigned int eSize = scene->getEntitiesNumber();
	for(i=0; i<eSize; i++)
	{
		MOEntity * entity = scene->getEntityByIndex(i);
		if(! entity->isActive())
		{
			if(isObjectSelected(entity))
				render->setColor3(MVector3(1, 1, 0));
			else
				render->setColor3(MVector3(0.5f));

			entity->updateMatrix();
			drawInvisibleEntity(entity);
		}
		else if(entity->isInvisible())
		{
			if(isObjectSelected(entity))
				render->setColor3(MVector3(1, 1, 0));
			else
				render->setColor3(MVector3(1.0f));

			drawInvisibleEntity(entity);
		}
	}

	// draw sounds
	unsigned int sSize = scene->getSoundsNumber();
	for(i=0; i<sSize; i++)
	{
		MOSound * sound = scene->getSoundByIndex(i);
		if(! sound->isActive())
			sound->updateMatrix();

		float size;
		if(! camera->isOrtho())
			size = (sound->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
		else
			size = camera->getFov() * editObjectSize;

		m_soundEntity->getMatrix()->loadIdentity();
		m_soundEntity->getMatrix()->setScale(sound->getTransformedScale() * MVector3(size));
		m_soundEntity->getMatrix()->setTranslationPart(sound->getTransformedPosition());

		if(isObjectSelected(sound))
			render->setColor3(MVector3(1, 1, 0));
		else
		{
			if(sound->isActive())
				render->setColor3(MVector3(1.0f));
			else
				render->setColor3(MVector3(0.5f));
		}

		render->pushMatrix();

		MMatrix4x4 matrix = *camera->getMatrix();
		matrix.setTranslationPart(MVector3());

		render->multMatrix(m_soundEntity->getMatrix());
		render->multMatrix(&matrix);

		drawSound();

		render->popMatrix();
	}

	// draw lights
	unsigned int lSize = scene->getLightsNumber();
	for(i=0; i<lSize; i++)
	{
		MOLight * light = scene->getLightByIndex(i);
		if(! light->isActive())
			light->updateMatrix();

		float size;
		if(! camera->isOrtho())
			size = (light->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
		else
			size = camera->getFov() * editObjectSize;

		m_lightEntity->getMatrix()->loadIdentity();
		m_lightEntity->getMatrix()->setScale(light->getTransformedScale() * MVector3(size));
		m_lightEntity->getMatrix()->setTranslationPart(light->getTransformedPosition());

		bool selected = isObjectSelected(light);
		if(selected)
			render->setColor3(MVector3(1, 1, 0));
		else
		{
			if(light->isActive())
				render->setColor3(MVector3(1.0f));
			else
				render->setColor3(MVector3(0.5f));
		}

		render->pushMatrix();

		MMatrix4x4 matrix = *camera->getMatrix();
		matrix.setTranslationPart(MVector3(0, 0, 0));

		render->multMatrix(m_lightEntity->getMatrix());
		render->multMatrix(&matrix);

		drawLight();

		render->popMatrix();

		// spot
		if(light->isVisible())
		{
			float spotAngle = light->getSpotAngle();
			if(spotAngle < 90)
			{
				if(selected)
					render->setColor4(MVector4(1, 1, 0, 0.5f));
				else
				{
					if(light->isActive())
						render->setColor4(MVector4(1, 1, 1, 0.5f));
					else
						render->setColor4(MVector4(0.5f, 0.5f, 0.5f, 0.5f));
				}

				size = size*10;
				if(size > light->getRadius()*0.25f)
					size = light->getRadius()*0.25f;

				MVector3 pos = light->getTransformedPosition();
				MVector3 dir1 = light->getRotatedVector(MVector3(0, 0, -1));//.getRotatedX(spotAngle)).getNormalized();
				MVector3 dir2 = light->getRotatedVector(MVector3(0, 0, -1));//.getRotatedX(-spotAngle)).getNormalized();

				MVector3 axis = light->getInverseRotatedVector(cameraAxis);
				axis.z = 0;
				axis = light->getRotatedVector(axis);

				matrix.loadIdentity();
				matrix.setRotationAxis(spotAngle, axis);
				dir1 = matrix * dir1;

				matrix.loadIdentity();
				matrix.setRotationAxis(-spotAngle, axis);
				dir2 = matrix * dir2;

				dir1.normalize();
				dir2.normalize();

				beginDraw(M_PRIMITIVE_LINES);
				pushVertex(pos);
				pushVertex(pos + dir1*size);
				pushVertex(pos);
				pushVertex(pos + dir2*size);
				endDraw(render);

				beginDraw(M_PRIMITIVE_LINE_LOOP);
				for(int x=0; x<360; x+=10)
				{
					MVector3 dir = MVector3(0, 0, -1).getRotatedX(spotAngle);
					dir.rotateZ(x);
					dir = light->getRotatedVector(dir).getNormalized();
					pushVertex(pos + dir*size);
				}
				endDraw(render);
			}
		}
	}

	// draw cameras
	unsigned int cSize = scene->getCamerasNumber();
	for(i=0; i<cSize; i++)
	{
		MOCamera * oCamera = scene->getCameraByIndex(i);
		if(! oCamera->isActive())
			oCamera->updateMatrix();

		if(! camera->isOrtho())
		{
			float scale = (oCamera->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
			MMatrix4x4 scaleMatrix;
			scaleMatrix.setScale(MVector3(scale, scale, scale));
			(*m_cameraEntity->getMatrix()) = (*oCamera->getMatrix()) * scaleMatrix;
		}
		else
		{
			float scale = camera->getFov() * editObjectSize;
			MMatrix4x4 scaleMatrix;
			scaleMatrix.setScale(MVector3(scale, scale, scale));
			(*m_cameraEntity->getMatrix()) = (*oCamera->getMatrix()) * scaleMatrix;
		}

		if(isObjectSelected(oCamera))
			render->setColor3(MVector3(1, 1, 0));
		else
		{
			if(oCamera->isActive())
				render->setColor3(MVector3(1.0f));
			else
				render->setColor3(MVector3(0.5f));
		}

		render->pushMatrix();
		render->multMatrix(m_cameraEntity->getMatrix());

		drawCamera();

		render->popMatrix();
	}

	// draw empty texts
	unsigned int tSize = scene->getTextsNumber();
	for(i=0; i<tSize; i++)
	{
		MOText * text = scene->getTextByIndex(i);
		if(! text->isActive())
			text->updateMatrix();

		MVector3 min = text->getBoundingBox()->min;
		MVector3 max = text->getBoundingBox()->max;

		if((min.x == 0 && max.x == 0 && min.y == 0 && max.y == 0) || (! text->isActive()))
		{
			render->pushMatrix();
			render->multMatrix(text->getMatrix());

			render->enableTexture();
			m_emptyText.setFontRef(text->getFontRef());
			renderer->drawText(&m_emptyText);
			render->disableTexture();

			if(isObjectSelected(text))
			{
				render->setColor3(MVector3(1, 1, 0));
				drawBoundingBox(m_emptyText.getBoundingBox());
			}

			render->popMatrix();
		}
	}

	
	// draw armatures
	render->disableDepthTest();
	for(i=0; i<eSize; i++)
	{
		MOEntity * entity = scene->getEntityByIndex(i);
		if(entity->isActive() && !entity->isInvisible() && isObjectSelected(entity))
		{
			render->setColor3(MVector3(0.75f));
			drawArmature(entity);
		}
	}
	render->enableDepthTest();
	
	
	// draw selected objects
	if((! mouse->isLeftButtonPushed()) || (m_currentAxis == M_AXIS_NONE))
	{
		render->enableBlending();
		render->setBlendingMode(M_BLENDING_ALPHA);

		MObject3d * object;
		MOEntity * entity;
		MOText * text;

		unsigned int oSize = getSelectedObjectsNumber();
		for(i=0; i<oSize; i++)
		{
			object = getSelectedObjectByIndex(i);

			if(! object->isVisible())
				continue;

			switch(object->getType())
			{
                case M_OBJECT3D_ENTITY:
				{
					entity = (MOEntity *)object;

					render->setColor4(MVector4(1, 1, 0, 0.2f));
					render->pushMatrix();
					render->multMatrix(entity->getMatrix());
					drawBoundingBox(entity->getBoundingBox());
					render->popMatrix();
				}
                    break;
                case M_OBJECT3D_TEXT:
				{
					text = (MOText *)object;

					render->setColor4(MVector4(1, 1, 0, 0.2f));
					render->pushMatrix();
					render->multMatrix(text->getMatrix());
					drawBoundingBox(text->getBoundingBox());
					render->popMatrix();
				}
			}
		}
	}

	if(getSelectedObjectsNumber() > 0)
	{
		render->enableDepthTest();

		// clear z buffer
		render->clear(M_BUFFER_DEPTH);
		camera->enable();

		switch(getTransformMode())
		{
            case M_TRANSFORM_ROTATION:
                updateSelectionCenter();
                drawEditRotation(camera);
                break;
            case M_TRANSFORM_POSITION:
                updateSelectionCenter();
                drawEditPosition(camera);
                break;
            case M_TRANSFORM_SCALE:
                updateSelectionCenter();
                drawEditScale(camera);
                break;

            default:
                break;
		}
	}
}

void Maratis::logicLoop(void)
{
	MaratisUI * UI = MaratisUI::getInstance();
	Maratis * maratis = Maratis::getInstance();
	MEngine * engine = MEngine::getInstance();
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	// game
	MGame * game = engine->getGame();
	if(game)
	{
		if(game->isRunning())
		{
			game->update();
			return;
		}
	}

	// update scene
	scene->update();

	// update behaviors
	unsigned int i;
	unsigned int oSize = scene->getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		MObject3d * object = scene->getObjectByIndex(i);
		if(! object->isActive())
			continue;

		object->updateBehaviors();
	}

	// update objects matrices
	scene->updateObjectsMatrices();

	// play
	if(UI->isPlaying())
	{
		int tPos = maratis->getTimelinePos() + 1;
		if(tPos > maratis->getTimelineMax())
			tPos = maratis->getTimelineMin();
		maratis->setTimelinePos(tPos);
	}

	// scene current frame
	int sceneCurrentFrame = maratis->getTimelinePos();
	scene->setCurrentFrame(sceneCurrentFrame);

	// entities current frame
	unsigned int eSize = scene->getEntitiesNumber();
	for(i=0; i<eSize; i++)
	{
		MOEntity * entity = scene->getEntityByIndex(i);
		if(! entity->isActive())
			continue;

		MMesh * mesh = entity->getMesh();
		if(! mesh)
			continue;

		if(mesh->getAnimsRangesNumber() > 0)
		{
			unsigned int animId = entity->getAnimationId();
			entity->changeAnimation(animId);

			MAnimRange * range = &mesh->getAnimsRanges()[animId];

			int start = range->start;
			int end = range->end;
			int scale = end - start;

			int currentFrame;
			if(UI->isPlaying() && (scale > 0) && (range->loops != 0))
			{
				currentFrame = start + (sceneCurrentFrame % scale);
				if(range->loops > 0)
				{
					if(sceneCurrentFrame >= end + scale*(range->loops - 1))
						currentFrame = end;
				}
			}
			else
			{
				currentFrame = start + sceneCurrentFrame;
				if(currentFrame > end)
					currentFrame = end;
			}

			entity->setCurrentFrame((float)currentFrame);
		}
	}
}

void Maratis::graphicLoop(void)
{
	MaratisUI * UI = MaratisUI::getInstance();
	MWindow * window = MWindow::getInstance();
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// game
	MGame * game = MEngine::getInstance()->getGame();
	if(game)
	{
		if(game->isRunning())
		{
			render->disableScissorTest();
			render->setViewport(0, 0, window->getWidth(), window->getHeight());
			game->draw();
			return;
		}
	}

	// draw gui
	MGui::getInstance()->draw();

	// setup render
	render->disableScissorTest();
	render->enableTexture();

	// 2d mode
	set2dMode(render);

	if(window->isMouseOverWindow())
	{
		// info text
		UI->drawInfoText();

		// info transform
		UI->drawInfoTransform();
	}
}
