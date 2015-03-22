//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//  Neo, Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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

// For linking Glew
#define GLEW_STATIC

// GL, TODO : remove opengl call (should use MRenderingContext to be fully
// virtual)
#ifdef _WIN32
#include <glew.h>
#elif __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <sys/stat.h>
#elif(defined(LINUX) && !defined(USE_GLES))
#include <glew.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#endif

// Maratis Common
#ifdef USE_GLES
#include <ES2Context.h>
#else
#include <GLContext.h>
#endif

#include <ALContext.h>
#include <BulletContext.h>
#include <WinContext.h>
#include <ImageLoader.h>
#include <SoundLoader.h>
#include <FreetypeLoader.h>
#include <BinFontLoader.h>
#include <BinMeshLoader.h>

#include <NeoCore.h>
#include <NeoEngine.h>
#include <Window/Mouse.h>
#include <LookAtBehavior.h>
#include <FollowBehavior.h>
#include <LuaBehavior.h>
#include <ParticleSystemBehavior.h>
#include <LuaScript.h>
#include <Input.h>
#include <LevelLoad.h>
#include <LevelSave.h>
#include <MeshLoad.h>
#include <MeshSave.h>
#include <PackageManagerNPK.h>
#include <Project.h>
#include <StandardRenderer.h>
#include <FixedRenderer.h>

#include "Maratis.h"
#include <Window/Window.h>

// Bins
#include "../Bins/FontBin.h"
#include "../Bins/MeshBin.h"

// publisher
#include "../Publish/Publisher.h"

// assimp loader
#include "../Loaders/AssimpMeshLoader.h"

// Profiler
#include <Shiny.h>

using namespace Neo;

// add ext if not
void fileExtension(char *out, const char *in, const char *ext)
{
	int extLen = strlen(ext);

	int extId = strlen(in) - extLen;
	if (extId < 0)
		extId = 0;

	char copy[256];

	// *.level
	if (strncmp(in + extId, ext, extLen))
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
    Vector3 * m_vertices;
    Vector3 * m_normals;

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

    Vector3 * getVertices(unsigned int size)
    {
        if(size > m_verticesNumber)
        {
            SAFE_DELETE_ARRAY(m_vertices);
            m_vertices = new Vector3[size];
            m_verticesNumber = size;
        }

        return m_vertices;
    }

    Vector3 * getNormals(unsigned int size)
    {
        if(size > m_normalsNumber)
        {
            SAFE_DELETE_ARRAY(m_normals);
            m_normals = new Vector3[size];
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
m_renderer(NULL),
m_selectionLock(false),
m_snapToGrid(false),
m_snapDistance(0.0)
{
    sprintf(m_windowTitle, "Neo");
    m_titleChanged = false;

    // tempDir
    {
        NeoWindow * window = NeoWindow::getInstance();

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
        m_soundContext = new ALContext();
        m_physics = new BulletContext();
        m_script = new LuaScript();
        m_input = new Input();
        m_system = new MWinContext();
        m_level = new Level();
        m_game = new NeoGame();
        m_packageManager = new MPackageManagerNPK();

        m_physics->setSimulationQuality(2);

        NeoEngine * engine = NeoEngine::getInstance();

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
    }

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

void Maratis::initRenderer()
{
    if(m_render) return;
#ifndef USE_GLES
    m_render = new GLContext();
#else
	m_render = new ES2Context();
#endif
    NeoEngine::getInstance()->setRenderingContext(m_render);
    m_render->init();
}

void Maratis::publish(void)
{
    if(strcmp(m_currentProject, "") != 0)
    {
        Publisher * publisher = Publisher::getInstance();
        publisher->publish(m_currentProject);
    }
}

void Maratis::changeRenderer(const char * name)
{
    NeoEngine * engine = NeoEngine::getInstance();
    RendererManager * rendererManager = engine->getRendererManager();

    RendererCreator * renderer = rendererManager->getRendererByName(name);
    if(renderer && strcmp(engine->getRenderer()->getName(), name) != 0)
    {
        if(m_renderer)
            m_renderer->destroy();
        m_renderer = renderer->getNewRenderer();
        engine->setRenderer(m_renderer);
    }
}

void Maratis::autoSave(void)
{
    NeoEngine * engine = NeoEngine::getInstance();

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

    NeoEngine * engine = NeoEngine::getInstance();
    Level * level = engine->getLevel();

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
    engine->getLevel()->getCurrentScene()->updateObjectsMatrices();
}

void Maratis::redo(void)
{
    if((m_undo+1) >= m_undoNumber)
        return;

    NeoEngine * engine = NeoEngine::getInstance();
    Level * level = engine->getLevel();

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
    engine->getLevel()->getCurrentScene()->updateObjectsMatrices();
}

void Maratis::clearUndo(void)
{
    m_undo = 0;
    m_undoLevel = 0;
    m_undoNumber = 0;
}

MeshRef * Maratis::loadEditorMesh(const char * filename)
{
    Mesh * mesh = Mesh::getNew();
    if(xmlMeshLoad(filename, mesh))
    {
        MeshRef * ref = MeshRef::getNew(mesh, filename);
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
#ifndef USE_GLES
    const char * version = (const char *)glGetString(GL_VERSION);
    if(version)
    {
        sscanf(version, "%d", &GLversion);
    }
#else
    GLversion = 2;
#endif

    // MEngine
    {
        NeoEngine * engine = NeoEngine::getInstance();

        // loaders
        engine->getImageLoader()->addLoader(M_loadImage); // image loader
        engine->getSoundLoader()->addLoader(M_loadSound); // sound loader
        engine->getLevelLoader()->addLoader(xmlLevelLoad); // level loader
        engine->getFontLoader()->addLoader(M_loadFont); // font loader
        engine->getFontLoader()->addLoader(M_loadBinFont); // bin font loader

        // add behaviors
        engine->getBehaviorManager()->addBehavior(LookAtBehavior::getStaticName(), M_OBJECT3D, LookAtBehavior::getNew);
        engine->getBehaviorManager()->addBehavior(FollowBehavior::getStaticName(), M_OBJECT3D, FollowBehavior::getNew);
        engine->getBehaviorManager()->addBehavior(LuaBehavior::getStaticName(), M_OBJECT3D, LuaBehavior::getNew);
        engine->getBehaviorManager()->addBehavior(ParticleSystemBehavior::getStaticName(), M_OBJECT3D, ParticleSystemBehavior::getNew);

        // add renderers
        if(GLversion >= 2)
            engine->getRendererManager()->addRenderer(StandardRenderer::getStaticName(), StandardRenderer::getNew);
        engine->getRendererManager()->addRenderer(FixedRenderer::getStaticName(), FixedRenderer::getNew);

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
        Scene * scene = m_level->addNewScene();
        scene->setName("Scene-1");

        // game
        engine->setGame(m_game);

        // set default renderer
        if(m_renderer == NULL)
        {
            if(GLversion >= 2)
                m_renderer = new StandardRenderer();
            else
                m_renderer = new FixedRenderer();
        }
        engine->setRenderer(m_renderer);
    }

    // view entities
    m_sphereEntity = new OEntity	(loadEditorMesh("gui/meshs/sphere.mesh"));
    m_coneEntity = new OEntity		(loadEditorMesh("gui/meshs/cone.mesh"));
    m_cubeEntity = new OEntity		(loadEditorMesh("gui/meshs/box.mesh"));
    m_planeEntity = new OEntity	(loadEditorMesh("gui/meshs/plane.mesh"));
    m_xEntity = new OEntity		(loadEditorMesh("gui/meshs/x.mesh"));
    m_yEntity = new OEntity		(loadEditorMesh("gui/meshs/y.mesh"));
    m_zEntity = new OEntity		(loadEditorMesh("gui/meshs/z.mesh"));
    m_xcircleEntity = new OEntity	(loadEditorMesh("gui/meshs/xcircle.mesh"));
    m_ycircleEntity = new OEntity	(loadEditorMesh("gui/meshs/ycircle.mesh"));
    m_zcircleEntity = new OEntity	(loadEditorMesh("gui/meshs/zcircle.mesh"));
    m_lightEntity = new OEntity	(loadEditorMesh("gui/meshs/light.mesh"));
    m_cameraEntity = new OEntity	(loadEditorMesh("gui/meshs/camera.mesh"));
    m_soundEntity = new OEntity	(loadEditorMesh("gui/meshs/sound.mesh"));
}

void Maratis::clear(void)
{
    NeoEngine * engine = NeoEngine::getInstance();

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
    m_perspectiveVue.setClippingNear(0.1);
    m_perspectiveVue.setClippingFar(100000);
    m_perspectiveVue.setPosition(Vector3(0, -200, 200));
    m_perspectiveVue.setEulerRotation(Vector3(40, 0, 0));
    m_perspectiveVue.updateMatrix();

    m_selectionCenter = Vector3(0, 0, 0);
    m_viewCenter = m_selectionCenter;
    clearSelectedObjects();
}

void Maratis::loadGamePlugin(void)
{
    NeoWindow * window = NeoWindow::getInstance();

    char gameFile[256];

    #ifdef WIN32
        getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.dll");
    #elif __APPLE__
        getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.dylib");
    #elif linux
        getGlobalFilename(gameFile, window->getWorkingDirectory(), "Game.so");
    #endif

    SAFE_DELETE(m_gamePlugin);
    m_gamePlugin = new Plugin();
    m_gamePlugin->load(gameFile);
}

void Maratis::getNewObjectName(const char *objectName, char *name)
{
	Level *level = NeoEngine::getInstance()->getLevel();
	Scene *scene = level->getCurrentScene();

	int pos = strlen(objectName) - 1;
	// Count number at end of string
	while (pos >= 0 && isalpha(objectName[pos]) == 0)
	{
		pos--;
	}

	if (pos < 0)
		pos = strlen(objectName);

	// Do not append the new number straight to the
	// existing one. Prevents names like "Entity11111" when
	// copying objects.
	char *shortName = (char *)malloc(pos + 2);
	strncpy(shortName, objectName, pos + 1);
	shortName[pos + 2] = '\0';

	unsigned int count = 0;
	int size = scene->getObjectsNumber();
	for (int i = 0; i < size; i++)
	{
		Object3d *object = scene->getObjectByIndex(i);
		if (object->getName())
		{
			if (strcmp(name, object->getName()) == 0)
			{
				// name already exist
				count++;
				sprintf(name, "%s%d", shortName, count);
				i = -1;
			}
		}
	}

	free(shortName);
}

Object3d* Maratis::duplicateObject(Object3d *object)
{
	Scene *scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

	OLight *light;
	OEntity *entity;
	OCamera *camera;
	OSound *sound;
	OText *text;
	Object3d *group;
	char name[256];

	strcpy(name, object->getName());
	Object3d* copiedObject;
	
	switch (object->getType())
	{
		case M_OBJECT3D_ENTITY:
		{
			getNewObjectName(object->getName(), name);

			copiedObject = entity = scene->addNewEntity(*(OEntity *)object);
			entity->setName(name);
		}
		break;
		case M_OBJECT3D_LIGHT:
		{
			getNewObjectName(object->getName(), name);

			copiedObject = light = scene->addNewLight(*(OLight *)object);
			light->setName(name);
		}
		break;
		case M_OBJECT3D_CAMERA:
		{
			getNewObjectName(object->getName(), name);

			copiedObject = camera = scene->addNewCamera(*(OCamera *)object);
			camera->setName(name);
		}
		break;
		case M_OBJECT3D_SOUND:
		{
			getNewObjectName(object->getName(), name);

			copiedObject = sound = scene->addNewSound(*(OSound *)object);
			sound->setName(name);
		}
		break;
		case M_OBJECT3D_TEXT:
		{
			getNewObjectName(object->getName(), name);
			copiedObject = text = scene->addNewText(*(OText *)object);
			text->setName(name);
		}
		break;
		case M_OBJECT3D:
		{
			getNewObjectName(object->getName(), name);
			copiedObject = group = scene->addNewGroup(*object);
			group->setName(name);
		}
		break;
	}

	Object3d* childCopy;
	// Copy all children
	for (int i = 0; i < object->getChildsNumber(); i++)
	{
		childCopy = duplicateObject(object->getChild(i));
		
		unlinkTwoObjects(childCopy->getParent(), childCopy);
		childCopy->linkTo(copiedObject);
	}

	return copiedObject;
}

void Maratis::duplicateSelectedObjects()
{
    autoSave();

    Scene * scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

    // get current objects number
    unsigned int objStart = scene->getObjectsNumber();
    unsigned int i;
    unsigned int oSize = m_selectedObjects.size();
    if(oSize == 0)
        return;

    for(i = 0; i < oSize; i++)
    {
		duplicateObject(m_selectedObjects[i]);
	}

	Object3d* object = NULL;
	
    // select duplicated objects
    m_selectedObjects.clear();
    oSize = scene->getObjectsNumber();
    for(i=objStart; i<oSize; i++)
    {
        object = scene->getObjectByIndex(i);
        m_selectedObjects.push_back(object);
    }
}

void Maratis::linkSelectedObjects(void)
{
    autoSave();

    unsigned int i;
    unsigned int oSize = m_selectedObjects.size();
    if(oSize < 2)
        return;

    Object3d * parent = m_selectedObjects[0];
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
        Object3d * object = m_selectedObjects[i];
        unlinkTwoObjects(object->getParent(), object);
    }
}

void Maratis::linkTwoObjects(Object3d *parent, Object3d *child)
{
    if(parent == NULL || child == NULL)
        return;

    child->linkTo(parent);

    // local matrix
    Matrix4x4 localMatrix = parent->getMatrix()->getInverse() * (*child->getMatrix());

    child->setPosition(localMatrix.getTranslationPart());
    child->setEulerRotation(localMatrix.getEulerAngles());

    float xSize = localMatrix.getRotatedVector3(Vector3(1, 0, 0)).getLength();
    float ySize = localMatrix.getRotatedVector3(Vector3(0, 1, 0)).getLength();
    float zSize = localMatrix.getRotatedVector3(Vector3(0, 0, 1)).getLength();

    child->setScale(Vector3(xSize, ySize, zSize));
}

void Maratis::unlinkTwoObjects(Object3d *parent, Object3d *child)
{
    if(parent == NULL || child == NULL)
        return;

    child->unLink();

    // matrix
    Matrix4x4 * matrix = child->getMatrix();

    child->setPosition(matrix->getTranslationPart());
    child->setEulerRotation(matrix->getEulerAngles());

    float xSize = matrix->getRotatedVector3(Vector3(1, 0, 0)).getLength();
    float ySize = matrix->getRotatedVector3(Vector3(0, 1, 0)).getLength();
    float zSize = matrix->getRotatedVector3(Vector3(0, 0, 1)).getLength();

    child->setScale(Vector3(xSize, ySize, zSize));
}

void Maratis::deleteSelectedObjects(void)
{
    autoSave();

    Level * level = NeoEngine::getInstance()->getLevel();
    Scene * scene = level->getCurrentScene();

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

    Scene * scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

    OCamera * camera = scene->addNewCamera();
    camera->setName(name);

    camera->setPosition(*getSelectionCenter());
    camera->setEulerRotation(Vector3(90, 0, 0));
    camera->updateMatrix();

    clearSelectedObjects();
    addSelectedObject(camera);
}

void Maratis::addLight(void)
{
    autoSave();

    char name[256] = "Light0";
    getNewObjectName("Light", name);

    Scene * scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

    OLight * light = scene->addNewLight();
    light->setName(name);

    light->setRadius(1000);
    light->setPosition(*getSelectionCenter());
    light->updateMatrix();

    clearSelectedObjects();
    addSelectedObject(light);
}

void Maratis::addGroup(void)
{
	autoSave();
	char name[256] = "Group0";
	getNewObjectName("Group", name);

	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

	Object3d* object = scene->addNewGroup();
	object->setName(name);

	clearSelectedObjects();
	addSelectedObject(object);
}

void Maratis::okAddEntity(const char * filename)
{
    if(filename)
    {
        Maratis * maratis = Maratis::getInstance();
        Level * level = NeoEngine::getInstance()->getLevel();
        Scene * scene = level->getCurrentScene();

        MeshRef * meshRef = level->loadMesh(filename);
        if(meshRef && meshRef->getMesh()->getSubMeshsNumber() > 0)
        {
            char name[256] = "Entity0";
            maratis->getNewObjectName("Entity", name);

            // add entity
            OEntity * entity = scene->addNewEntity(meshRef);
            entity->setName(name);

            // set entity position
            entity->setPosition(*maratis->getSelectionCenter());
            entity->updateMatrix();

            maratis->clearSelectedObjects();
            maratis->addSelectedObject(entity);
        }
    }
}

void Maratis::okAddSound(const char * filename)
{
    if(filename)
    {
        Maratis * maratis = Maratis::getInstance();
        Level * level = NeoEngine::getInstance()->getLevel();
        Scene * scene = level->getCurrentScene();

        SoundRef * soundRef = level->loadSound(filename);
        if(soundRef)
        {
            char name[256] = "Sound0";
            maratis->getNewObjectName("Sound", name);

            // add sound
            OSound * sound = scene->addNewSound(soundRef);
            sound->setName(name);

            // set position
            sound->setPosition(*maratis->getSelectionCenter());
            sound->updateMatrix();

            maratis->clearSelectedObjects();
            maratis->addSelectedObject(sound);
        }
    }
}

void Maratis::okAddFont(const char * filename)
{
    if(filename)
    {
        Maratis * maratis = Maratis::getInstance();
        Level * level = NeoEngine::getInstance()->getLevel();
        Scene * scene = level->getCurrentScene();

        // font test
        FontRef * fontRef = level->loadFont(filename);
        if(fontRef)
        {
            OText * text = scene->addNewText(fontRef);

            char name[256] = "Text0";
            maratis->getNewObjectName("Text", name);

            text->setName(name);
            text->setText("Text");

            // set position
            text->setPosition(*maratis->getSelectionCenter());
            text->setEulerRotation(Vector3(180, 0, 0));
            text->updateMatrix();

            maratis->clearSelectedObjects();
            maratis->addSelectedObject(text);
        }
    }
}

void Maratis::addEntity(void)
{
    autoSave();

    NeoWindow * window = NeoWindow::getInstance();

    char startPath[256];
    getGlobalFilename(startPath, window->getWorkingDirectory(), "meshs");
}

void Maratis::addSound(void)
{
    autoSave();

    NeoWindow * window = NeoWindow::getInstance();

    char startPath[256];
    getGlobalFilename(startPath, window->getWorkingDirectory(), "sounds");
}

void Maratis::addText(void)
{
    autoSave();

    NeoWindow * window = NeoWindow::getInstance();

    char startPath[256];
    getGlobalFilename(startPath, window->getWorkingDirectory(), "fonts");
}

void Maratis::updateTitle(const char * additional)
{
    char levelDir[256];
    char levelName[256];

    getRepertory(levelDir, m_currentLevel);
    getLocalFilename(levelName, levelDir, m_currentLevel);

    if(additional)
        sprintf(m_windowTitle, "Neo - %s - %s%s", m_currentProject, levelName, additional);
    else
        sprintf(m_windowTitle, "Neo - %s - %s", m_currentProject, levelName);

    m_titleChanged = true;
}

void Maratis::okNewProject(const char * filename)
{
    NeoWindow * window = NeoWindow::getInstance();
    Maratis * maratis = Maratis::getInstance();
    SystemContext* system = NeoEngine::getInstance()->getSystemContext();

    char file[256];
    fileExtension(file, filename, ".mproj");
    Project proj;
    if(proj.saveXML(file))
    {
        // create empty dir
        char rep[256];
        char src[256];
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

        getGlobalFilename(src, system->getWorkingDirectory(), "NeoPlayer");
        getGlobalFilename(dir, rep, "NeoPlayer");

        copyFile(src, dir);

		getGlobalFilename(src, system->getWorkingDirectory(), "LuaApi");
		getGlobalFilename(dir, rep, "scripts/SDK");

		copyDirectory(src, dir);

#ifndef WIN32
        chmod(dir, S_IRWXU);
#endif
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

}

void Maratis::okImportExternal(const char * filename)
{
    M_importAssimpMeshes(filename);
}

void Maratis::newProject(void)
{

}

void Maratis::okLoadProject(const char * filename)
{
    Maratis * maratis = Maratis::getInstance();
    maratis->loadProject(filename);
}

void Maratis::loadProject(void)
{

}

void Maratis::loadProject(const char * filename)
{
    NeoWindow * window = NeoWindow::getInstance();

    if(! filename)
        return;

    NeoEngine::getInstance()->getLevel()->clear();
    NeoEngine::getInstance()->getLevel()->clearScenes();

    // load project file
    Project proj;
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
        if(MPackageManager* pPackMan = NeoEngine::getInstance()->getPackageManager())
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
    NeoEngine * engine = NeoEngine::getInstance();

    // init maratis
    initVue();

    // clear level
    Level * level = engine->getLevel();
    level->clear();

    // add first scene
    Scene * scene = level->addNewScene();
    scene->setName("Scene-1");

    strcpy(m_currentLevel, "");
    updateTitle();
}

bool Maratis::loadLevel(const char * filename)
{
    NeoEngine * engine = NeoEngine::getInstance();

    if(! filename)
        return false;

    // load level
    Level * level = engine->getLevel();
    if(engine->getLevelLoader()->loadData(filename, level))
    {
        strcpy(m_currentLevel, filename);

        // init maratis
        initVue();

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
    NeoWindow * window = NeoWindow::getInstance();

    char startPath[256];
    getGlobalFilename(startPath, window->getWorkingDirectory(), "levels");
}

void Maratis::save()
{
    NeoEngine * engine = NeoEngine::getInstance();

    //if(m_currentLevel[0] == '\0')
    //{
        //saveAs();
    //}
    //else
    {
        xmlLevelSave(engine->getLevel(), m_currentLevel);
        Project proj;
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
    NeoEngine * engine = NeoEngine::getInstance();

    // save level
    char file[256];
    fileExtension(file, filename, ".level");
    if(xmlLevelSave(engine->getLevel(), file))
    {
        strcpy(maratis->m_currentLevel, file);
        Project proj;
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
    NeoWindow * window = NeoWindow::getInstance();

    char startPath[256];
    getGlobalFilename(startPath, window->getWorkingDirectory(), "levels");
}

void Maratis::addSelectedObject(Object3d * object)
{
	if(m_selectionLock) return;

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
    Vector3 lCenter = m_perspectiveVue.getInversePosition(m_viewCenter);

    // rotation
    m_perspectiveVue.addAxisAngleRotation(Vector3(1, 0, 0), -(float)mouse->getYDirection()*0.65f);

    Matrix4x4 matrix;
    Quaternion rotation = m_perspectiveVue.getRotation();
    matrix.setRotationAxis(rotation.getAngle(), rotation.getAxis());
    matrix.invert();

    Vector3 axis = matrix * Vector3(0, 0, 1);
    m_perspectiveVue.addAxisAngleRotation(axis, -(float)mouse->getXDirection()*0.65f);
    m_perspectiveVue.updateMatrix();

    // position
    Vector3 fCenter = m_perspectiveVue.getTransformedVector(lCenter);
    m_perspectiveVue.setPosition(m_perspectiveVue.getPosition() + (m_viewCenter - fCenter));

    m_perspectiveVue.updateMatrix();
}

void Maratis::panCurrentVue(void)
{
    MMouse * mouse = MMouse::getInstance();
    OCamera * camera = getPerspectiveVue();

    int * viewport = camera->getCurrentViewport();

    Vector3 vecX(1, 0, 0);
    Vector3 vecZ(0, 1, 0);
    Vector3 xAxis = camera->getRotatedVector(vecX);
    Vector3 zAxis = camera->getRotatedVector(vecZ);

    if(camera->isOrtho())
    {
        // position
        Vector3 position = camera->getPosition();
        float dFactor = camera->getFov() / (viewport[3]);

        camera->setPosition(position + ((xAxis * (-(float)mouse->getXDirection())) + (zAxis * (float)mouse->getYDirection()))*dFactor);
        camera->updateMatrix();

        return;
    }

    // position
    Vector3 position = camera->getPosition();
    Vector3 axis = camera->getRotatedVector(Vector3(0, 0, -1));

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
    OCamera * camera = getPerspectiveVue();
    Vector3 position = camera->getPosition();

    if(camera->isOrtho())
    {
        int dir = mouse->getWheelDirection();
        camera->setFov(camera->getFov() - (camera->getFov() * dir * 0.15f));

        Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;

        float factor = (camera->getFov() - 1);
        float dist = (m_viewCenter - position).dotProduct(cameraAxis);

        camera->setPosition(position + (cameraAxis * (dist - factor)));
        camera->updateMatrix();

        return;
    }

    // dir
    float dir = (-mouse->getWheelDirection()) * 20.0f;
    float dFactor = (m_viewCenter - position).getLength() * 0.01f;

    Vector3 vec(0, 0, -1);
    Vector3 axis = camera->getRotatedVector(vec);

    camera->setPosition(position - (axis * dir * dFactor));
    camera->updateMatrix();
}

void Maratis::switchCurrentVueMode(void)
{
    OCamera * camera = getPerspectiveVue();
    Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;
    Vector3 position = camera->getPosition();

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
    OCamera * camera = getPerspectiveVue();

    // set to ortho
    if(!camera->isOrtho())
        switchCurrentVueMode();

    // set vue
    switch(vue)
    {
        case 1:
            camera->setEulerRotation(Vector3(90, 0, 0));
            break;
        case 3:
            camera->setEulerRotation(Vector3(90, 0, 90));
            break;
        case 7:
            camera->setEulerRotation(Vector3(0, 0, 0));
            break;
        case 9:
            camera->setEulerRotation(Vector3(180, 0, 0));
            break;
    }

    camera->updateMatrix();
}

void Maratis::drawBoundingBox(Box3d * box)
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    Vector3 * min = &box->min;
    Vector3 * max = &box->max;

    beginDraw(PRIMITIVE_LINES);

    pushVertex(Vector3(min->x, min->y, min->z));
    pushVertex(Vector3(max->x, min->y, min->z));

    pushVertex(Vector3(min->x, min->y, min->z));
    pushVertex(Vector3(min->x, max->y, min->z));

    pushVertex(Vector3(min->x, min->y, min->z));
    pushVertex(Vector3(min->x, min->y, max->z));

    pushVertex(Vector3(max->x, max->y, max->z));
    pushVertex(Vector3(min->x, max->y, max->z));

    pushVertex(Vector3(max->x, max->y, max->z));
    pushVertex(Vector3(max->x, min->y, max->z));

    pushVertex(Vector3(max->x, max->y, max->z));
    pushVertex(Vector3(max->x, max->y, min->z));

    pushVertex(Vector3(max->x, max->y, min->z));
    pushVertex(Vector3(min->x, max->y, min->z));

    pushVertex(Vector3(max->x, min->y, max->z));
    pushVertex(Vector3(min->x, min->y, max->z));

    pushVertex(Vector3(max->x, max->y, min->z));
    pushVertex(Vector3(max->x, min->y, min->z));

    pushVertex(Vector3(min->x, max->y, max->z));
    pushVertex(Vector3(min->x, min->y, max->z));

    pushVertex(Vector3(min->x, max->y, max->z));
    pushVertex(Vector3(min->x, max->y, min->z));

    pushVertex(Vector3(max->x, min->y, max->z));
    pushVertex(Vector3(max->x, min->y, min->z));

    endDraw(render);
}

bool getNearestRaytracedDistance(Mesh * mesh, Matrix4x4 * matrix, const Vector3 & origin, const Vector3 & dest, float * distance, OEntity * entity = NULL)
{
    // animate armature
    if(entity && mesh->getArmature())
    {
        Armature * armature = mesh->getArmature();
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

    Matrix4x4 iMatrix = matrix->getInverse();

    Vector3 localOrigin = iMatrix * origin;
    Vector3 localDest = iMatrix * dest;

    Box3d * box = mesh->getBoundingBox();
    if(! isEdgeToBoxCollision(localOrigin, localDest, box->min, box->max))
        return false;

    bool raytraced = false;

    float dist;
    float nearDist;
    Vector3 I, intersectionPoint;
    Vector3 rayVector = localDest - localOrigin;

    // init near dist
    nearDist =  rayVector.getSquaredLength();

    unsigned int s;
    unsigned int sSize = mesh->getSubMeshsNumber();
    for(s=0; s<sSize; s++)
    {
        SubMesh * subMesh = &mesh->getSubMeshs()[s];
        box = subMesh->getBoundingBox();

        // skinning
        Vector3 * vertices = subMesh->getVertices();
        if(entity && mesh->getArmature() && subMesh->getSkinData())
        {
            unsigned int verticesSize = subMesh->getVerticesSize();
            Vector3 * skinVertices = MSkinCache::getInstance()->getVertices(verticesSize);
            computeSkinning(mesh->getArmature(), subMesh->getSkinData(), vertices, NULL, NULL, skinVertices, NULL, NULL);
            vertices = skinVertices;
        }

        if(isEdgeToBoxCollision(localOrigin, localDest, box->min, box->max))
        {
            unsigned int d;
            unsigned int dSize = subMesh->getDisplaysNumber();
            for(d=0; d<dSize; d++)
            {
                MaterialDisplay * display = subMesh->getDisplay(d);

                // indices
                void * indices = subMesh->getIndices();
                switch(subMesh->getIndicesType())
                {
                    case VAR_USHORT:
                    {
                        unsigned short * idx = (unsigned short *)indices;
                        indices = (void *)(idx + display->getBegin());
                        break;
                    }
                    case VAR_UINT:
                    {
                        unsigned int * idx = (unsigned int *)indices;
                        indices = (void *)(idx + display->getBegin());
                        break;
                    }

                    default:
                        break;
                }

                // BACK or FRONT and BACK, scan ray
                if((display->getCullMode() == CULL_BACK) || (display->getCullMode() == CULL_NONE))
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
                if((display->getCullMode() == CULL_FRONT) || (display->getCullMode() == CULL_NONE))
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

Object3d * Maratis::getNearestMesh(Scene * scene, const Vector3 & rayO, const Vector3 & rayD, Vector3 * intersectPoint)
{
    float distance = 0;
    Object3d * nearestObject = NULL;


    unsigned int i;
    unsigned int oSize = scene->getEntitiesNumber();
    for(i=0; i<oSize; i++)
    {
        OEntity * entity = scene->getEntityByIndex(i);

        if(! entity->isActive())
            continue;

        if(entity->isInvisible())
            continue;

        float dist;

        Matrix4x4 * matrix = entity->getMatrix();

        Mesh * mesh = entity->getMesh();
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

Object3d * Maratis::getNearestObject(Scene * scene, const Vector3 & rayO, const Vector3 & rayD, Vector3 * intersectPoint)
{
    // get camera
    OCamera * camera = getPerspectiveVue();

    int * viewport = camera->getCurrentViewport();
    float editObjectSize = 10.0f / (float)viewport[3];

    // scan objects
    float distance = 0;
    Object3d * nearestObject = NULL;

    unsigned int i;
    unsigned int oSize = scene->getObjectsNumber();
    for(i=0; i<oSize; i++)
    {
        Object3d * object = scene->getObjectByIndex(i);

        //if(! object->isActive())
        //	continue;

        float dist;
        switch(object->getType())
        {
            case M_OBJECT3D_ENTITY:
            {
                OEntity * entity = (OEntity *)object;
                Matrix4x4 * matrix = entity->getMatrix();

                Mesh * mesh = entity->getMesh();
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

                Matrix4x4 scaleMatrix;
                scaleMatrix.setScale(Vector3(scale, scale, scale));
                Matrix4x4 matrix = ((*object->getMatrix()) * scaleMatrix);

                Mesh * mesh = m_cameraEntity->getMesh();
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

                Matrix4x4 scaleMatrix;
                scaleMatrix.setScale(Vector3(scale, scale, scale));
                Matrix4x4 matrix = ((*object->getMatrix()) * scaleMatrix);

                Mesh * mesh = m_sphereEntity->getMesh();
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

                Matrix4x4 scaleMatrix;
                scaleMatrix.setScale(Vector3(scale, scale, scale)*1.75f);
                Matrix4x4 matrix = ((*object->getMatrix()) * scaleMatrix);

                Mesh * mesh = m_sphereEntity->getMesh();
                if(! getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &dist))
                    continue;
            }
                break;
            case M_OBJECT3D_TEXT:
            {
                OText * text = (OText*)object;
                Vector3 min = text->getBoundingBox()->min;
                Vector3 max = text->getBoundingBox()->max;

                if((min.x == 0 && max.x == 0 && min.y == 0 && max.y == 0) || (! text->isActive()))
                {
                    m_emptyText.setFontRef(text->getFontRef());
                    min = m_emptyText.getBoundingBox()->min;
                    max = m_emptyText.getBoundingBox()->max;
                }

                Matrix4x4 scaleMatrix;
                scaleMatrix.setScale(Vector3(max.x - min.x, max.y - min.y, 1));
                scaleMatrix.setTranslationPart(min);

                Matrix4x4 matrix = (*object->getMatrix()) * scaleMatrix;

                Mesh * mesh = m_planeEntity->getMesh();
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

    Level * level = NeoEngine::getInstance()->getLevel();
    Scene * scene = level->getCurrentScene();

    bool allSelect = (m_selectedObjects.size() == scene->getObjectsNumber());

    clearSelectedObjects();

    if(! allSelect)
    {
        unsigned int i;
        unsigned int size = scene->getObjectsNumber();
        for(i=0; i<size; i++)
            addSelectedObject(scene->getObjectByIndex(i));
    }
}

void Maratis::focusSelection(void)
{
    if(m_selectedObjects.empty())
        return;

    float distance = 0.0f;
    Vector3 center(0, 0, 0);
    for(std::vector <Object3d *>::iterator itObj = m_selectedObjects.begin(); itObj != m_selectedObjects.end(); ++itObj)
    {
        Object3d *pobj = *itObj;
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
                    OEntity *pentity = (OEntity*)pobj;
                    Box3d *pbox = pentity->getBoundingBox();

                    Vector3 scale = pentity->getTransformedScale();
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

    OCamera * camera = getPerspectiveVue();
    Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();
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
        Object3d * object = getSelectedObjectByIndex(i);
        object->setActive(! object->isActive());
    }
}

void Maratis::selectSameMesh(void)
{
    autoSave();

    Level * level = NeoEngine::getInstance()->getLevel();
    Scene * scene = level->getCurrentScene();

    std::vector <Mesh *> meshs;

    unsigned int i;
    unsigned int selSize = getSelectedObjectsNumber();
    for(i=0; i<selSize; i++)
    {
        Object3d * object = getSelectedObjectByIndex(i);
        if(object->getType() == M_OBJECT3D_ENTITY)
        {
            OEntity * entity = (OEntity *)object;
            Mesh * mesh = entity->getMesh();
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
        OEntity * entity = scene->getEntityByIndex(i);
        Mesh * mesh = entity->getMesh();
        for(j=0; j<meshsSize; j++){
            if(meshs[j] == mesh){
                addSelectedObject(entity);
                break;
            }
        }
    }
}

void Maratis::updateCurrentAxis()
{
    // get window
    NeoWindow * window = NeoWindow::getInstance();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    // get camera
    OCamera * camera = getPerspectiveVue();
    Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;

    // viewport
    int * viewport = camera->getCurrentViewport();

    // ray (origin and dest)
    Vector3 rayO = camera->getTransformedPosition();
    Vector3 rayD = camera->getUnProjectedPoint(Vector3(
                                                         (float)mouse->getXPosition(),
                                                         (float)(window->getHeight() - mouse->getYPosition()),
                                                         1));
    if(camera->isOrtho())
    {
        Vector3 point;
        isRayPlaneIntersection(rayD, -cameraAxis, rayO + (cameraAxis * camera->getClippingNear()), cameraAxis, &point);
        rayO = point;
    }

    rayD = rayO + ((rayD - rayO).getNormalized() * (camera->getClippingFar() - camera->getClippingNear()));

    float radius;
    Vector3 position;

    if(! camera->isOrtho())
    {
        radius = (camera->getFov() * 400) / (float)viewport[3];
        Vector3 cameraDir = (*getSelectionCenter()) - rayO;
        cameraDir.normalize();
        position = rayO + cameraDir*200;
    }
    else
    {
        radius = (camera->getFov() * 120) / (float)viewport[3];
        Vector3 point;
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
}

void Maratis::selectObjectsInMainView(Scene * scene, bool multipleSelection)
{
    // get window
    NeoWindow * window = NeoWindow::getInstance();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    // get camera
    OCamera * camera = getPerspectiveVue();
    Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;

    // viewport
    int * viewport = camera->getCurrentViewport();

    // ray (origin and dest)
    Vector3 rayO = camera->getTransformedPosition();
    Vector3 rayD = camera->getUnProjectedPoint(Vector3(
                                                         (float)mouse->getXPosition(),
                                                         (float)(window->getHeight() - mouse->getYPosition()),
                                                         1));

    if(camera->isOrtho())
    {
        Vector3 point;
        isRayPlaneIntersection(rayD, -cameraAxis, rayO + (cameraAxis * camera->getClippingNear()), cameraAxis, &point);
        rayO = point;
    }

    rayD = rayO + ((rayD - rayO).getNormalized() * (camera->getClippingFar() - camera->getClippingNear()));

    // transform
    if(!multipleSelection && (getSelectedObjectsNumber() > 0) && (getTransformMode() != M_TRANSFORM_MOUSE))
    {
        float radius;
        Vector3 position;

        if(! camera->isOrtho())
        {
            radius = (camera->getFov() * 400) / (float)viewport[3];
            Vector3 cameraDir = (*getSelectionCenter()) - rayO;
            cameraDir.normalize();
            position = rayO + cameraDir*200;
        }
        else
        {
            radius = (camera->getFov() * 120) / (float)viewport[3];
            Vector3 point;
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
    if(!multipleSelection)
        clearSelectedObjects();

    Object3d * nearestObject = getNearestObject(scene, rayO, rayD);

    if(nearestObject)
    {
        autoSave();
        addSelectedObject(nearestObject);
    }
}

void Maratis::updateViewCenter(void)
{
    // view center
    Level * level = NeoEngine::getInstance()->getLevel();
    Scene * scene = level->getCurrentScene();

    Vector3 rayO = m_perspectiveVue.getTransformedPosition();
    Vector3 rayD = m_perspectiveVue.getTransformedVector(Vector3(0, 0, -m_perspectiveVue.getClippingFar()));

    getNearestMesh(scene, rayO, rayD, &m_viewCenter);
}

void Maratis::updateSelectionCenter(void)
{
    Vector3 position(0, 0, 0);

    unsigned int i;
    unsigned int oSize = getSelectedObjectsNumber();
    for(i=0; i<oSize; i++)
    {
        Object3d * object = getSelectedObjectByIndex(i);
        position += object->getTransformedPosition();
    }

    if(oSize > 0)
        m_selectionCenter = position / (float)oSize;
}

void Maratis::drawGrid(Scene * scene)
{
    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

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
    Vector3 clearColor = Vector3(0.2f, 0.3f, 0.4f);
    Vector3 color1 = (clearColor * 0.8f);
    Vector3 color2 = (clearColor * 0.9f);
    Vector3 red =   (clearColor * 0.7f) + (Vector3(1, 0, 0) * 0.3f);
    Vector3 green = (clearColor * 0.7f) + (Vector3(0, 1, 0) * 0.3f);

    // draw lines
    render->setColor3(color2);
    beginDraw(PRIMITIVE_LINES);
    for(i=0; i<=gSize; i++)
    {
        float length = (float)(i*gridSpace);

        pushVertex(Vector3(-length, -gLength, 0));
        pushVertex(Vector3(-length,  gLength, 0));

        pushVertex(Vector3( length, -gLength, 0));
        pushVertex(Vector3( length,  gLength, 0));

        pushVertex(Vector3(-gLength, -length, 0));
        pushVertex(Vector3( gLength, -length, 0));

        pushVertex(Vector3(-gLength, length, 0));
        pushVertex(Vector3( gLength, length, 0));
    }
    endDraw(render);

    render->disableDepthTest();

    render->setColor3(color1);
    beginDraw(PRIMITIVE_LINES);
    for(i=0; i<=gSize; i+=gridSpace)
    {
        float length = (float)(i*gridSpace);

        pushVertex(Vector3(-length, -gLength, 0));
        pushVertex(Vector3(-length,  gLength, 0));

        pushVertex(Vector3( length, -gLength, 0));
        pushVertex(Vector3( length,  gLength, 0));

        pushVertex(Vector3(-gLength, -length, 0));
        pushVertex(Vector3( gLength, -length, 0));

        pushVertex(Vector3(-gLength, length, 0));
        pushVertex(Vector3( gLength, length, 0));
    }
    endDraw(render);

    // axis
    render->setColor3(red);
    beginDraw(PRIMITIVE_LINES);
    pushVertex(Vector3(-gLength, 0, 0));
    pushVertex(Vector3( gLength, 0, 0));
    endDraw(render);

    render->setColor3(green);
    beginDraw(PRIMITIVE_LINES);
    pushVertex(Vector3(0, -gLength, 0));
    pushVertex(Vector3(0,  gLength, 0));
    endDraw(render);
}

void Maratis::drawScaleAxis(M_AXIS axis, OCamera * camera, Matrix4x4 * matrix, const bool viewTest)
{
    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    Vector3 position = matrix->getTranslationPart();

    // normal
    Vector3 normal;
    Object3d * object = NULL;

    switch(axis)
    {
        case M_AXIS_X:
            normal = Vector3(1, 0, 0);
            object = m_xEntity;
            break;

        case M_AXIS_Y:
            normal = Vector3(0, 1, 0);
            object = m_yEntity;
            break;

        case M_AXIS_Z:
            normal = Vector3(0, 0, 1);
            object = m_zEntity;
            break;

        default:break;
    }

    // camera direction
    Vector3 cameraPos = camera->getTransformedPosition();
    Vector3 cameraDir;

    if(! camera->isOrtho())
    {
        cameraDir = position - cameraPos;
        cameraDir.normalize();
    }
    else
    {
        cameraDir = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;
    }

    // view test
    if(viewTest)
    {
        Vector3 axisNormal = matrix->getRotatedVector3(normal).getNormalized();

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
            render->rotate(Vector3(0, 1, 0), 90);
            break;

        case M_AXIS_Y:
            render->rotate(Vector3(1, 0, 0), -90);
            break;

        default:
            break;
    }

    beginDraw(PRIMITIVE_LINES);
    pushVertex(Vector3(0, 0, 0.3f));
    pushVertex(Vector3(0, 0, 0.9f));
    endDraw(render);

    render->translate(Vector3(0, 0, 0.9f));
    drawTriangles(m_cubeEntity->getMesh());

    render->popMatrix();
}

void Maratis::drawPositionAxis(M_AXIS axis, OCamera * camera, Matrix4x4 * matrix, const bool viewTest)
{
    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // get position from matrix
    Vector3 position = matrix->getTranslationPart();

    // normal
    Vector3 normal;
    Object3d * object = NULL;

    switch(axis)
    {
        case M_AXIS_X:
            normal = Vector3(1, 0, 0);
            object = m_xEntity;
            break;

        case M_AXIS_Y:
            normal = Vector3(0, 1, 0);
            object = m_yEntity;
            break;

        case M_AXIS_Z:
            normal = Vector3(0, 0, 1);
            object = m_zEntity;
            break;

        default:
            break;
    }

    // camera direction
    Vector3 cameraPos = camera->getTransformedPosition();
    Vector3 cameraDir;

    if(! camera->isOrtho())
    {
        cameraDir = position - cameraPos;
        cameraDir.normalize();
    }
    else
    {
        cameraDir = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();
    }

    // view test
    if(viewTest)
    {
        Vector3 axisNormal = matrix->getRotatedVector3(normal).getNormalized();

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
            render->rotate(Vector3(0, 1, 0), 90);
            break;

        case M_AXIS_Y:
            render->rotate(Vector3(1, 0, 0), -90);
            break;

        default:
            break;
    }

    beginDraw(PRIMITIVE_LINES);
    pushVertex(Vector3(0, 0, 0.3f));
    pushVertex(Vector3(0, 0, 0.9f));
    endDraw(render);

    render->translate(Vector3(0, 0, 0.9f));
    drawTriangles(m_coneEntity->getMesh());

    render->popMatrix();
}

void Maratis::drawRotationCircle(M_AXIS axis, OCamera * camera, Matrix4x4 * matrix, const bool zTest)
{
    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // normal
    Vector3 normal;
    Vector3 vector;

    Vector3 position = matrix->getTranslationPart();

    // camera direction
    Vector3 cameraPos = camera->getTransformedPosition();
    Vector3 cameraDir;

    if(! camera->isOrtho())
    {
        cameraDir = position - cameraPos;
        cameraDir.normalize();
    }
    else
    {
        cameraDir = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;
    }

    switch(axis)
    {
        case M_AXIS_X:
            normal = Vector3(1, 0, 0);
            vector = Vector3(0, 1, 0);
            break;

        case M_AXIS_Y:
            normal = Vector3(0, 1, 0);
            vector = Vector3(1, 0, 0);
            break;

        case M_AXIS_Z:
            normal = Vector3(0, 0, 1);
            vector = Vector3(1, 0, 0);
            break;

        default:
            break;
    }

    if(! zTest)
        render->disableDepthTest();
    else
    {
        Vector3 axisNormal = matrix->getRotatedVector3(normal).getNormalized();

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
    beginDraw(PRIMITIVE_LINE_STRIP);

    Vector3 vec;
    unsigned int i;
    for(i=0; i<=360; i+=10)
    {
        vec = vector.getRotatedAxis((double)i, normal);
        pushVertex(vec);
    }

    endDraw(render);
    render->popMatrix();
}

void Maratis::computeTransformDirection(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, const float distance, const Vector3 & axis)
{
    // get window
    NeoWindow * window = NeoWindow::getInstance();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    // intersection
    Vector3 intersection = rayO + ((rayD - rayO).getNormalized() * distance);

    // normal
    Vector3 normal = getTriangleNormal(position, intersection, position + axis);

    // mouse position
    m_tMousePosition = Vector2((float)mouse->getXPosition(), (float)mouse->getYPosition());

    // vector direction
    Vector3 pPoint = camera->getProjectedPoint(intersection - (normal * 100));
    pPoint.y = window->getHeight() - pPoint.y;

    m_tVectorDirection = (Vector2(pPoint.x, pPoint.y) - m_tMousePosition).getNormalized();
}

void Maratis::computeTransformPlane(OCamera * camera, const Vector3 & position, const Vector3 & axis)
{
    // get window
    NeoWindow * window = NeoWindow::getInstance();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    // plane
    Vector3 up = camera->getRotatedVector(Vector3(1, 1, 1));
    m_tPlane = getTriangleNormal(Vector3(0, 0, 0), axis, up);

    // center position
    Vector3 pPoint = camera->getProjectedPoint(position);
    pPoint.y = window->getHeight() - pPoint.y;

    m_tCenterPosition = Vector2(pPoint.x, pPoint.y);

    // vector direction
    pPoint = camera->getProjectedPoint(position + (axis * 100));
    pPoint.y = window->getHeight() - pPoint.y;

    m_tVectorDirection = Vector2(pPoint.x, pPoint.y) - m_tCenterPosition;
    m_tVectorDirection.normalize();

    // mouse position
    m_tMousePosition = Vector2((float)mouse->getXPosition(), (float)mouse->getYPosition());

    // offset direction
    m_tOffsetDirection = m_tMousePosition - m_tCenterPosition;
}

M_AXIS Maratis::selectEditScale(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, float radius)
{
    // axis
    Vector3 radiusScale = Vector3(radius, radius, radius);
    Vector3 xAxis(1, 0, 0);
    Vector3 yAxis(0, 1, 0);
    Vector3 zAxis(0, 0, 1);

    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // rotation
    Vector3 rotation(0, 0, 0);
    if(oSize == 1)
    {
        Object3d * object = getSelectedObjectByIndex(0);
        Vector3 worldScale = object->getTransformedScale();

        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

        Matrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

        rotation = matrix.getEulerAngles();

        xAxis = matrix.getRotatedVector3(xAxis);
        yAxis = matrix.getRotatedVector3(yAxis);
        zAxis = matrix.getRotatedVector3(zAxis);
    }

    // matrice
    Matrix4x4 matrix;
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
        Mesh * mesh = m_zEntity->getMesh();
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
        Mesh * mesh = m_yEntity->getMesh();
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
        Mesh * mesh = m_xEntity->getMesh();
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
        Scene * scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

        unsigned int i;
        for(i=0; i<oSize; i++)
        {
            Object3d * object = getSelectedObjectByIndex(i);
            if(getNearestObject(scene, rayO, rayD) == object)
            {
                computeTransformPlane(camera, position, Vector3(0, 0, 0));
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

M_AXIS Maratis::selectEditPosition(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, float radius)
{
    // axis
    Vector3 radiusScale = Vector3(radius, radius, radius);
    Vector3 xAxis(1, 0, 0);
    Vector3 yAxis(0, 1, 0);
    Vector3 zAxis(0, 0, 1);

    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // rotation
    Vector3 rotation(0, 0, 0);
    if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
    {
        Object3d * object = getSelectedObjectByIndex(0);
        Vector3 worldScale = object->getTransformedScale();

        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

        Matrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

        rotation = matrix.getEulerAngles();

        xAxis = matrix.getRotatedVector3(xAxis);
        yAxis = matrix.getRotatedVector3(yAxis);
        zAxis = matrix.getRotatedVector3(zAxis);
    }

    // matrice
    Matrix4x4 matrix;
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
        Mesh * mesh = m_zEntity->getMesh();
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
        Mesh * mesh = m_yEntity->getMesh();
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
        Mesh * mesh = m_xEntity->getMesh();
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
        Scene * scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();
        Object3d * nearestObj = getNearestObject(scene, rayO, rayD);

        unsigned int i;
        for(i=0; i<oSize; i++)
        {
            Object3d * object = getSelectedObjectByIndex(i);
            if(object == nearestObj)
            {
                computeTransformPlane(camera, position, Vector3(0, 0, 0));
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

M_AXIS Maratis::selectEditRotation(OCamera * camera, const Vector3 & rayO, const Vector3 & rayD, const Vector3 & position, const float radius)
{
    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    Vector3 radiusScale = Vector3(radius, radius, radius);
    Vector3 xAxis(1, 0, 0);
    Vector3 yAxis(0, 1, 0);
    Vector3 zAxis(0, 0, 1);

    // rotation
    Vector3 rotation(0, 0, 0);
    if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
    {
        Object3d * object = getSelectedObjectByIndex(0);
        Vector3 worldScale = object->getTransformedScale();

        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

        Matrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

        rotation = matrix.getEulerAngles();

        xAxis = matrix.getRotatedVector3(xAxis);
        yAxis = matrix.getRotatedVector3(yAxis);
        zAxis = matrix.getRotatedVector3(zAxis);
    }

    Matrix4x4 matrix;
    matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
    matrix.setTranslationPart(position);
    matrix.scale(radiusScale);

    // raytrace cirle meshs
    float distance;
    float cDist = (position - rayO).getLength() * 1.1f;

    // z
    {
        Mesh * mesh = m_zcircleEntity->getMesh();
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
        Mesh * mesh = m_ycircleEntity->getMesh();
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
        Mesh * mesh = m_xcircleEntity->getMesh();
        if(getNearestRaytracedDistance(mesh, &matrix, rayO, rayD, &distance))
            if(distance < cDist)
            {
                computeTransformDirection(camera, rayO, rayD, position, distance, xAxis);
                autoSave();
                return M_AXIS_X;
            }
    }

    // view axis
    Scene * scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

    unsigned int i;
    for(i=0; i<oSize; i++)
    {
        Object3d * object = getSelectedObjectByIndex(i);
        if(getNearestObject(scene, rayO, rayD) == object)
        {
            computeTransformPlane(camera, position, Vector3(0, 0, 0));
            autoSave();
            return M_AXIS_VIEW;
        }
    }

    return M_AXIS_NONE;
}

void Maratis::drawEditPosition(OCamera * camera)
{
    // viewport
    int * viewport = camera->getCurrentViewport();

    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    float radius;
    Vector3 position;

    if(! camera->isOrtho())
    {
        radius = (camera->getFov() * 400) / (float)viewport[3];
        Vector3 cameraPos = camera->getTransformedPosition();
        Vector3 cameraDir = (*getSelectionCenter()) - cameraPos;
        cameraDir.normalize();
        position = cameraPos + cameraDir*200;
    }
    else
    {
        radius = (camera->getFov() * 120) / (float)viewport[3];
        Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;
        Vector3 point;
        isRayPlaneIntersection((*getSelectionCenter()), -cameraAxis, camera->getTransformedPosition(), cameraAxis, &point);
        position = point + cameraAxis*200;
    }

    Vector3 radiusScale = Vector3(radius, radius, radius);

    // rotation
    Vector3 rotation(0, 0, 0);
    if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
    {
        Object3d * object = getSelectedObjectByIndex(0);
        Vector3 worldScale = object->getTransformedScale();

        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

        rotation = ((*object->getMatrix()) * iScaleMatrix).getEulerAngles();
    }

    // matrix
    Matrix4x4 matrix;
    matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
    matrix.setTranslationPart(position);
    matrix.scale(radiusScale);

    // axis
    if(mouse->isLeftButtonPushed() && (m_currentAxis != M_AXIS_NONE))
    {
        switch(m_currentAxis)
        {
            case M_AXIS_X:
                render->setColor3(Vector3(1, 1, 0));
                drawPositionAxis(M_AXIS_X, camera, &matrix, false);
                return;

            case M_AXIS_Y:
                render->setColor3(Vector3(1, 1, 0));
                drawPositionAxis(M_AXIS_Y, camera, &matrix, false);
                return;

            case M_AXIS_Z:
                render->setColor3(Vector3(1, 1, 0));
                drawPositionAxis(M_AXIS_Z, camera, &matrix, false);
                return;

            case M_AXIS_VIEW:
                return;

            default:
                break;
        }
    }

    if(m_currentAxis != M_AXIS_NONE)
    {
        switch(m_currentAxis)
        {
            case M_AXIS_X:
                render->setColor3(Vector3(1, 1, 0));
                drawPositionAxis(M_AXIS_X, camera, &matrix, false);
                render->setColor3(Vector3(0, 1, 0));
                drawPositionAxis(M_AXIS_Y, camera, &matrix);
                render->setColor3(Vector3(0, 0, 1));
                drawPositionAxis(M_AXIS_Z, camera, &matrix);
                return;

            case M_AXIS_Y:
                render->setColor3(Vector3(1, 1, 0));
                drawPositionAxis(M_AXIS_Y, camera, &matrix, false);
                render->setColor3(Vector3(1, 0, 0));
                drawPositionAxis(M_AXIS_X, camera, &matrix);
                render->setColor3(Vector3(0, 0, 1));
                drawPositionAxis(M_AXIS_Z, camera, &matrix);
                return;

            case M_AXIS_Z:
                render->setColor3(Vector3(1, 1, 0));
                drawPositionAxis(M_AXIS_Z, camera, &matrix, false);
                render->setColor3(Vector3(1, 0, 0));
                drawPositionAxis(M_AXIS_X, camera, &matrix);
                render->setColor3(Vector3(0, 1, 0));
                drawPositionAxis(M_AXIS_Y, camera, &matrix);
                return;

            default:
                break;
        }
    }


    // draw axis
    render->setColor3(Vector3(1, 0, 0));
    drawPositionAxis(M_AXIS_X, camera, &matrix);
    render->setColor3(Vector3(0, 1, 0));
    drawPositionAxis(M_AXIS_Y, camera, &matrix);
    render->setColor3(Vector3(0, 0, 1));
    drawPositionAxis(M_AXIS_Z, camera, &matrix);
}

void Maratis::drawEditScale(OCamera * camera)
{
    // viewport
    int * viewport = camera->getCurrentViewport();

    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    float radius;
    Vector3 position;

    if(! camera->isOrtho())
    {
        radius = (camera->getFov() * 400) / (float)viewport[3];
        Vector3 cameraPos = camera->getTransformedPosition();
        Vector3 cameraDir = (*getSelectionCenter()) - cameraPos;
        cameraDir.normalize();
        position = cameraPos + cameraDir*200;
    }
    else
    {
        radius = (camera->getFov() * 120) / (float)viewport[3];
        Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;
        Vector3 point;
        isRayPlaneIntersection((*getSelectionCenter()), -cameraAxis, camera->getTransformedPosition(), cameraAxis, &point);
        position = point + cameraAxis*200;
    }

    Vector3 radiusScale = Vector3(radius, radius, radius);

    // rotation
    Vector3 rotation(0, 0, 0);
    if(oSize == 1)
    {
        Object3d * object = getSelectedObjectByIndex(0);
        Vector3 worldScale = object->getTransformedScale();

        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

        rotation = ((*object->getMatrix()) * iScaleMatrix).getEulerAngles();
    }

    // matrix
    Matrix4x4 matrix;
    matrix.setRotationEuler(rotation.x, rotation.y, rotation.z);
    matrix.setTranslationPart(position);
    matrix.scale(radiusScale);

    // axis
    if(mouse->isLeftButtonPushed() && (m_currentAxis != M_AXIS_NONE))
    {
        switch(m_currentAxis)
        {
            case M_AXIS_X:
                render->setColor3(Vector3(1, 1, 0));
                drawScaleAxis(M_AXIS_X, camera, &matrix, false);
                return;

            case M_AXIS_Y:
                render->setColor3(Vector3(1, 1, 0));
                drawScaleAxis(M_AXIS_Y, camera, &matrix, false);
                return;

            case M_AXIS_Z:
                render->setColor3(Vector3(1, 1, 0));
                drawScaleAxis(M_AXIS_Z, camera, &matrix, false);
                return;

            case M_AXIS_VIEW:
                return;

            default:
                break;
        }
    }

    if(m_currentAxis != M_AXIS_NONE)
    {
        switch(m_currentAxis)
        {
            case M_AXIS_X:
                render->setColor3(Vector3(1, 1, 0));
                drawScaleAxis(M_AXIS_X, camera, &matrix, false);
                render->setColor3(Vector3(0, 1, 0));
                drawScaleAxis(M_AXIS_Y, camera, &matrix);
                render->setColor3(Vector3(0, 0, 1));
                drawScaleAxis(M_AXIS_Z, camera, &matrix);
                return;

            case M_AXIS_Y:
                render->setColor3(Vector3(1, 1, 0));
                drawScaleAxis(M_AXIS_Y, camera, &matrix, false);
                render->setColor3(Vector3(1, 0, 0));
                drawScaleAxis(M_AXIS_X, camera, &matrix);
                render->setColor3(Vector3(0, 0, 1));
                drawScaleAxis(M_AXIS_Z, camera, &matrix);
                return;

            case M_AXIS_Z:
                render->setColor3(Vector3(1, 1, 0));
                drawScaleAxis(M_AXIS_Z, camera, &matrix, false);
                render->setColor3(Vector3(1, 0, 0));
                drawScaleAxis(M_AXIS_X, camera, &matrix);
                render->setColor3(Vector3(0, 1, 0));
                drawScaleAxis(M_AXIS_Y, camera, &matrix);
                return;

            default:
                break;
        }
    }

    // draw axis
    render->setColor3(Vector3(1, 0, 0));
    drawScaleAxis(M_AXIS_X, camera, &matrix);
    render->setColor3(Vector3(0, 1, 0));
    drawScaleAxis(M_AXIS_Y, camera, &matrix);
    render->setColor3(Vector3(0, 0, 1));
    drawScaleAxis(M_AXIS_Z, camera, &matrix);
}

void Maratis::drawEditRotation(OCamera * camera)
{
    // viewport
    int * viewport = camera->getCurrentViewport();

    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // get engine
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    float radius;
    Vector3 position;
    Vector3 cameraPos = camera->getTransformedPosition();
    Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();;

    if(! camera->isOrtho())
    {
        radius = (camera->getFov() * 400) / (float)viewport[3];
        Vector3 cameraDir = (*getSelectionCenter()) - cameraPos;
        cameraDir.normalize();
        position = cameraPos + cameraDir*200;
    }
    else
    {
        radius = (camera->getFov() * 120) / (float)viewport[3];
        Vector3 point;
        isRayPlaneIntersection((*getSelectionCenter()), -cameraAxis, camera->getTransformedPosition(), cameraAxis, &point);
        position = point + cameraAxis*200;
    }

    Vector3 radiusScale = Vector3(radius, radius, radius);

    // rotation
    Vector3 rotation(0, 0, 0);
    if((oSize == 1) && (getOrientationMode() == M_ORIENTATION_LOCAL))
    {
        Object3d * object = getSelectedObjectByIndex(0);
        Vector3 worldScale = object->getTransformedScale();

        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / worldScale.x, 1.0f / worldScale.y, 1.0f / worldScale.z));

        rotation = ((*object->getMatrix()) * iScaleMatrix).getEulerAngles();
    }

    // matrix
    Matrix4x4 matrix;
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
    render->setBlendingMode(BLENDING_ALPHA);

    // draw view circle
    render->setColor4(Vector4(1, 1, 1, 0.2f));

    Matrix4x4 eyeMatrix;
    if(! camera->isOrtho())
    {
        Quaternion quat;
        quat.setFromVectors(Vector3(0, 0, 1), (*getSelectionCenter()) - cameraPos);
        eyeMatrix.setRotationAxis(quat.getAngle(), quat.getAxis());
    }
    else
    {
        Quaternion rotation = camera->getRotation();
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
                render->setColor4(Vector4(1, 1, 1, 0.2f));
                drawRotationCircle(M_AXIS_X, camera, &matrix, false);
                render->setColor3(Vector3(1, 1, 0));
                drawRotationCircle(M_AXIS_X, camera, &matrix);
                return;

            case M_AXIS_Y:
                render->setColor4(Vector4(1, 1, 1, 0.2f));
                drawRotationCircle(M_AXIS_Y, camera, &matrix, false);
                render->setColor3(Vector3(1, 1, 0));
                drawRotationCircle(M_AXIS_Y, camera, &matrix);
                return;

            case M_AXIS_Z:
                render->setColor4(Vector4(1, 1, 1, 0.2f));
                drawRotationCircle(M_AXIS_Z, camera, &matrix, false);
                render->setColor3(Vector3(1, 1, 0));
                drawRotationCircle(M_AXIS_Z, camera, &matrix);
                return;

            case M_AXIS_VIEW:
                return;

            default:
                break;
        }
    }

    render->setColor3(Vector3(1, 0, 0));
    drawRotationCircle(M_AXIS_X, camera, &matrix);
    render->setColor3(Vector3(0, 1, 0));
    drawRotationCircle(M_AXIS_Y, camera, &matrix);
    render->setColor3(Vector3(0, 0, 1));
    drawRotationCircle(M_AXIS_Z, camera, &matrix);
}

void Maratis::transformRotation(void)
{
    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // get mouse
    MMouse * mouse = MMouse::getInstance();
    Vector2 dir = Vector2((float)mouse->getXDirection(), (float)mouse->getYDirection());

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
            Vector2 vec = (m_tMousePosition - m_tCenterPosition).getNormalized();
            angle = Vector2(-vec.y, vec.x).dotProduct(dir);
        }
            break;

        default:
            break;
    }

    // get camera
    OCamera * camera = getPerspectiveVue();

    // rotate
    if(getOrientationMode() == M_ORIENTATION_WORLD)
    {
        Vector3 axis;
        Vector3 * position = getSelectionCenter();

        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = Vector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = Vector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = Vector3(0, 0, 1);
                break;

            case M_AXIS_VIEW:
            {
                if(camera->isOrtho())
                    axis = camera->getRotatedVector(Vector3(0, 0, -1));
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
            Object3d * object = getSelectedObjectByIndex(i);

            if(object->hasParent())
            {
                Object3d * parent = object->getParent();
                if(isObjectSelected(parent))
                    continue;

                // translate
                Vector3 vec = object->getTransformedPosition() - (*position);
                vec.rotateAxis((double)angle, axis);

                Vector3 localPos = parent->getInversePosition((*position) + vec);
                object->setPosition(localPos);

                // rotate
                Vector3 iAxis = object->getInverseRotatedVector(axis) / object->getTransformedScale();

                object->addAxisAngleRotation(iAxis, angle);
            }
            else
            {
                // translate
                Vector3 vec = object->getTransformedPosition() - (*position);
                vec.rotateAxis((double)angle, axis);
                object->setPosition((*position) + vec);

                // rotate
                Vector3 iAxis = object->getInverseRotatedVector(axis) / object->getTransformedScale();
                object->addAxisAngleRotation(iAxis, angle);
            }

            // update matrix
            object->updateMatrix();
        }
    }
    else
    {
        Vector3 axis;

        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = Vector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = Vector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = Vector3(0, 0, 1);
                break;

            case M_AXIS_VIEW:
            {
                if(camera->isOrtho())
                    axis = camera->getRotatedVector(Vector3(0, 0, -1));
            }
                break;

            default:
                break;
        }

        unsigned int i;
        for(i=0; i<oSize; i++)
        {
            Object3d * object = getSelectedObjectByIndex(i);

            if(m_currentAxis == M_AXIS_VIEW)
            {
                if(! camera->isOrtho())
                    axis = (object->getTransformedPosition() - camera->getTransformedPosition()).getNormalized();

                Vector3 iAxis = object->getInverseRotatedVector(axis) / object->getTransformedScale();
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
    Vector3 * position = getSelectionCenter();

    // mouse direction
    Vector2 mouseDir = Vector2((float)mouse->getXDirection(), (float)mouse->getYDirection());

    // view axis
    if(m_currentAxis == M_AXIS_VIEW)
    {
        // scale factor
        Vector2 dir = (m_tMousePosition - m_tCenterPosition).getNormalized();
        float scaleFactor = 1.0f + dir.dotProduct(mouseDir) * 0.01f;

        unsigned int i;
        for(i=0; i<oSize; i++)
        {
            Object3d * object = getSelectedObjectByIndex(i);

            if(getOrientationMode() == M_ORIENTATION_WORLD)
            {
                if(object->hasParent())
                {
                    Object3d * parent = object->getParent();
                    if(isObjectSelected(parent))
                        continue;

                    Vector3 worldPos = object->getTransformedPosition();
                    Vector3 localPos = parent->getInversePosition((*position) + ((worldPos - (*position)) * scaleFactor));
                    object->setPosition(localPos);
                }
                else
                {
                    Vector3 worldPos = object->getTransformedPosition();
                    object->setPosition((*position) + ((worldPos - (*position)) * scaleFactor));
                }
            }

            Vector3 scale = object->getScale() * scaleFactor;

            object->setScale(scale);
            object->updateMatrix();
        }

        return;
    }

    // axis
    Vector3 axis;
    if(oSize > 1)
    {
        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = Vector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = Vector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = Vector3(0, 0, 1);
                break;

            default:
                break;
        }
    }
    else
    {
        Object3d * object = getSelectedObjectByIndex(0);

        // matrix
        Vector3 scale = object->getTransformedScale();
        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z));
        Matrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = matrix.getRotatedVector3(Vector3(1, 0, 0));
                break;

            case M_AXIS_Y:
                axis = matrix.getRotatedVector3(Vector3(0, 1, 0));
                break;

            case M_AXIS_Z:
                axis = matrix.getRotatedVector3(Vector3(0, 0, 1));
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
        Object3d * object = getSelectedObjectByIndex(i);
        Vector3 worldPos = object->getTransformedPosition();

        Vector3 scale = object->getScale();
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

void Maratis::transformPosition(Vector2 delta)
{
    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // get window
    NeoWindow * window = NeoWindow::getInstance();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    // get camera
    OCamera * camera = getPerspectiveVue();

    // position
    updateSelectionCenter();
    Vector3 * position = getSelectionCenter();

    // mouse position
    Vector2 mousePos((float)mouse->getXPosition(), (float)mouse->getYPosition());
    mousePos = mousePos - m_tOffsetDirection;

    // view axis
    if(m_currentAxis == M_AXIS_VIEW)
    {
        Vector3 viewAxis = camera->getRotatedVector(Vector3(0, 0, -1));

        // ray (origin and dest)
        Vector3 rayO = camera->getTransformedPosition();
        Vector3 rayD = camera->getUnProjectedPoint(
                                                    Vector3((float)mousePos.x, (float)(window->getHeight() - mousePos.y), 1)
                                                    );

        if(camera->isOrtho())
        {
            Vector3 point;
            isRayPlaneIntersection(rayD, -viewAxis, rayO, viewAxis, &point);
            rayO = point;
            rayD = (rayD - rayO).getNormalized();
        }
        else
        {
            rayD = (rayD - rayO).getNormalized();
        }

        // intersection
        Vector3 point;
        isRayPlaneIntersection(rayO, rayD, (*position), viewAxis, &point);

        unsigned int i;
        for(i=0; i<oSize; i++)
        {
            Object3d * object = getSelectedObjectByIndex(i);
            Vector3 worldPos = object->getTransformedPosition();

            if(object->hasParent())
            {
                Object3d * parent = object->getParent();
                if(! isObjectSelected(parent))
                {
                    Vector3 localPos = parent->getInversePosition(point + (worldPos - (*position)));
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
    Vector3 axis;
    if((getOrientationMode() == M_ORIENTATION_WORLD) || (oSize > 1))
    {
        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = Vector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = Vector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = Vector3(0, 0, 1);
                break;

            default:
                break;
        }
    }
    else
    {
        Object3d * object = getSelectedObjectByIndex(0);

        // matrix
        Vector3 scale = object->getTransformedScale();
        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z));
        Matrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = matrix.getRotatedVector3(Vector3(1, 0, 0));
                break;

            case M_AXIS_Y:
                axis = matrix.getRotatedVector3(Vector3(0, 1, 0));
                break;

            case M_AXIS_Z:
                axis = matrix.getRotatedVector3(Vector3(0, 0, 1));
                break;

            default:
                break;
        }
    }

    // mouse position on axis
    Vector2 axisNormal(-m_tVectorDirection.y, m_tVectorDirection.x);
    float axisMouseDist = (mousePos - m_tCenterPosition).dotProduct(axisNormal);
    mousePos = mousePos - (axisNormal * axisMouseDist);

    Vector3 viewAxis = camera->getRotatedVector(Vector3(0, 0, -1));

    // ray (origin and dest)
    Vector3 rayO = camera->getTransformedPosition();
    Vector3 rayD = camera->getUnProjectedPoint(
                                                Vector3(mousePos.x, (window->getHeight() - mousePos.y), 0.499238f)
                                                );

    if(camera->isOrtho())
    {
        Vector3 point;
        isRayPlaneIntersection(rayD, -viewAxis, rayO, viewAxis, &point);
        rayO = point;
        rayD = (rayD - rayO).getNormalized();
    }
    else
    {
        rayD = (rayD - rayO).getNormalized();
    }

    // intersection
    Vector3 point;
    isRayPlaneIntersection(rayO, rayD, (*position), m_tPlane, &point);

    float distance = axis.dotProduct((point) - (*position));

    unsigned int i;
    for(i=0; i<oSize; i++)
    {
        Object3d * object = getSelectedObjectByIndex(i);
        Vector3 worldPos = object->getTransformedPosition();

        if(object->hasParent())
        {
            Object3d * parent = object->getParent();
            if(! isObjectSelected(parent))
            {
                Vector3 localPos = parent->getInversePosition(worldPos + (axis * distance));
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

void Maratis::transformPosition(void)
{
    // objects
    unsigned int oSize = getSelectedObjectsNumber();

    // get window
    NeoWindow * window = NeoWindow::getInstance();

    // get mouse
    MMouse * mouse = MMouse::getInstance();

    // get camera
    OCamera * camera = getPerspectiveVue();

    // position
    updateSelectionCenter();
    Vector3 * position = getSelectionCenter();

    // mouse position
    Vector2 mousePos((float)mouse->getXPosition(), (float)mouse->getYPosition());
    mousePos = mousePos - m_tOffsetDirection;

    // view axis
    if(m_currentAxis == M_AXIS_VIEW)
    {
        Vector3 viewAxis = camera->getRotatedVector(Vector3(0, 0, -1));

        // ray (origin and dest)
        Vector3 rayO = camera->getTransformedPosition();
        Vector3 rayD = camera->getUnProjectedPoint(
                                                    Vector3((float)mousePos.x, (float)(window->getHeight() - mousePos.y), 1)
                                                    );

        if(camera->isOrtho())
        {
            Vector3 point;
            isRayPlaneIntersection(rayD, -viewAxis, rayO, viewAxis, &point);
            rayO = point;
            rayD = (rayD - rayO).getNormalized();
        }
        else
        {
            rayD = (rayD - rayO).getNormalized();
        }

        // intersection
        Vector3 point;
        isRayPlaneIntersection(rayO, rayD, (*position), viewAxis, &point);

        unsigned int i;
        for(i=0; i<oSize; i++)
        {
            Object3d * object = getSelectedObjectByIndex(i);
            Vector3 worldPos = object->getTransformedPosition();

            if(object->hasParent())
            {
                Object3d * parent = object->getParent();
                if(! isObjectSelected(parent))
                {
                    Vector3 localPos = parent->getInversePosition(point + (worldPos - (*position)));
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
    Vector3 axis;
    if((getOrientationMode() == M_ORIENTATION_WORLD) || (oSize > 1))
    {
        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = Vector3(1, 0, 0);
                break;

            case M_AXIS_Y:
                axis = Vector3(0, 1, 0);
                break;

            case M_AXIS_Z:
                axis = Vector3(0, 0, 1);
                break;

            default:
                break;
        }
    }
    else
    {
        Object3d * object = getSelectedObjectByIndex(0);

        // matrix
        Vector3 scale = object->getTransformedScale();
        Matrix4x4 iScaleMatrix;
        iScaleMatrix.setScale(Vector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z));
        Matrix4x4 matrix = (*object->getMatrix()) * iScaleMatrix;

        switch(m_currentAxis)
        {
            case M_AXIS_X:
                axis = matrix.getRotatedVector3(Vector3(1, 0, 0));
                break;

            case M_AXIS_Y:
                axis = matrix.getRotatedVector3(Vector3(0, 1, 0));
                break;

            case M_AXIS_Z:
                axis = matrix.getRotatedVector3(Vector3(0, 0, 1));
                break;

            default:
                break;
        }
    }

    // mouse position on axis
    Vector2 axisNormal(-m_tVectorDirection.y, m_tVectorDirection.x);
    float axisMouseDist = (mousePos - m_tCenterPosition).dotProduct(axisNormal);
    mousePos = mousePos - (axisNormal * axisMouseDist);

    Vector3 viewAxis = camera->getRotatedVector(Vector3(0, 0, -1));

    // ray (origin and dest)
    Vector3 rayO = camera->getTransformedPosition();
    Vector3 rayD = camera->getUnProjectedPoint(
                                                Vector3(mousePos.x, (window->getHeight() - mousePos.y), 0.499238f)
                                                );

    if(camera->isOrtho())
    {
        Vector3 point;
        isRayPlaneIntersection(rayD, -viewAxis, rayO, viewAxis, &point);
        rayO = point;
        rayD = (rayD - rayO).getNormalized();
    }
    else
    {
        rayD = (rayD - rayO).getNormalized();
    }

    // intersection
    Vector3 point;
    isRayPlaneIntersection(rayO, rayD, (*position), m_tPlane, &point);

    float distance = axis.dotProduct((point) - (*position));

    // When snap to grid is enabled.
    if(m_snapToGrid && fabs(distance) < m_snapDistance)
    {
    	return;
    }

    if(m_snapToGrid)
    {
    	if(distance < 0)
    		distance = -m_snapDistance;
    	else
    		distance = m_snapDistance;

    }

    unsigned int i;
    for(i=0; i<oSize; i++)
    {
        Object3d * object = getSelectedObjectByIndex(i);
        Vector3 worldPos = object->getTransformedPosition();

        if(object->hasParent())
        {
            Object3d * parent = object->getParent();
            if(! isObjectSelected(parent))
            {
                Vector3 localPos = parent->getInversePosition(worldPos + (axis * distance));
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

bool Maratis::isObjectSelected(Object3d * object)
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

void Maratis::drawInvisibleEntity(OEntity * entity)
{
    // HACK opengl
#ifndef USE_GLES
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();
    Mesh * mesh = entity->getMesh();
    if(mesh)
    {
        render->pushMatrix();
        render->multMatrix(entity->getMatrix());

        // draw
        drawTriangles(mesh);

        render->popMatrix();
    }

    // HACK opengl
#ifndef USE_GLES
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}

void Maratis::drawCamera(Scene * scene, OCamera * camera)
{
    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // clear z buffer
    render->enableDepthTest();
    render->clear(BUFFER_DEPTH);

    // draw scene
    camera->enable();
    scene->draw(camera);
    scene->drawObjectsBehaviors();
}

void Maratis::drawTriangles(Mesh * mesh)
{
    // get render
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    // subMeshs
    unsigned int s;
    unsigned sSize = mesh->getSubMeshsNumber();
    for(s=0; s<sSize; s++)
    {
        SubMesh * subMesh = &mesh->getSubMeshs()[s];

        render->disableLighting();
        render->disableNormalArray();
        render->disableColorArray();
        render->disableTexCoordArray();

        // vertices
        render->enableVertexArray();
        render->setVertexPointer(VAR_FLOAT, 3, subMesh->getVertices());

        unsigned int i;
        unsigned int displayNumber = subMesh->getDisplaysNumber();
        for(i=0; i<displayNumber; i++)
        {
            MaterialDisplay * display = subMesh->getDisplay(i);
            if(! display->isVisible())
                continue;

            // begin / size
            unsigned int begin = display->getBegin();
            unsigned int size = display->getSize();

            // display properties
            PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
            CULL_MODES cullMode = display->getCullMode();

            // cull mode
            render->setCullMode(cullMode);

            // indices
            void * indices = subMesh->getIndices();

            // draw
            if(indices)
            {
                switch(subMesh->getIndicesType())
                {
                    case VAR_USHORT:
                        render->drawElement(primitiveType, size, VAR_USHORT, (unsigned short*)indices + begin);
                        break;

                    case VAR_UINT:
                        render->drawElement(primitiveType, size, VAR_UINT, (unsigned int*)indices + begin);
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
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    Mesh * mesh = m_lightEntity->getMesh();
    unsigned short * indices = (unsigned short *)mesh->getSubMeshs()[0].getIndices();
    Vector3 * vertices = mesh->getSubMeshs()[0].getVertices();

    unsigned int i;
    for(i=0; i<mesh->getSubMeshs()[0].getIndicesSize(); i+=3)
    {
        beginDraw(PRIMITIVE_LINES);

        pushVertex(vertices[indices[i+0]]);
        pushVertex(vertices[indices[i+1]]);

        pushVertex(vertices[indices[i+1]]);
        pushVertex(vertices[indices[i+2]]);
        endDraw(render);
    }
}

void Maratis::drawCamera(void)
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    Mesh * mesh = m_cameraEntity->getMesh();
    unsigned short * indices = (unsigned short *)mesh->getSubMeshs()[0].getIndices();
    Vector3 * vertices = mesh->getSubMeshs()[0].getVertices();

    unsigned int i;
    for(i=0; i<mesh->getSubMeshs()[0].getIndicesSize(); i+=3)
    {
        beginDraw(PRIMITIVE_LINES);

        pushVertex(vertices[indices[i+0]]);
        pushVertex(vertices[indices[i+1]]);

        pushVertex(vertices[indices[i+1]]);
        pushVertex(vertices[indices[i+2]]);
        endDraw(render);
    }
}

void Maratis::drawSound(void)
{
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    Mesh * mesh = m_soundEntity->getMesh();
    unsigned short * indices = (unsigned short *)mesh->getSubMeshs()[0].getIndices();
    Vector3 * vertices = mesh->getSubMeshs()[0].getVertices();

    unsigned int i;
    for(i=0; i<mesh->getSubMeshs()[0].getIndicesSize(); i+=3)
    {
        beginDraw(PRIMITIVE_LINES);

        pushVertex(vertices[indices[i+0]]);
        pushVertex(vertices[indices[i+1]]);

        pushVertex(vertices[indices[i+1]]);
        pushVertex(vertices[indices[i+2]]);
        endDraw(render);
    }
}

void Maratis::drawArmature(OEntity * entity)
{
    NeoEngine * engine = NeoEngine::getInstance();
    RenderingContext * render = engine->getRenderingContext();

    Mesh * mesh = entity->getMesh();
    if(mesh)
    {
        Armature * armature = mesh->getArmature();
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

            Vector3 iVue = m_perspectiveVue.getPosition();

            beginDraw(PRIMITIVE_LINES);

            unsigned int b, bSize = armature->getBonesNumber();
            for(b=0; b<bSize; b++)
            {
                OBone * bone = armature->getBone(b);

                Vector3 pos = entity->getTransformedVector(bone->getTransformedPosition());
                float size = (pos - iVue).getLength() * 0.01f;

                pushVertex(pos + bone->getUniformRotatedVector(Vector3(-size, 0, 0)));
                pushVertex(pos + bone->getUniformRotatedVector(Vector3( size, 0, 0)));
                pushVertex(pos + bone->getUniformRotatedVector(Vector3(0, -size, 0)));
                pushVertex(pos + bone->getUniformRotatedVector(Vector3(0,  size, 0)));
                pushVertex(pos + bone->getUniformRotatedVector(Vector3(0, 0, -size)));
                pushVertex(pos + bone->getUniformRotatedVector(Vector3(0, 0,  size)));
            }

            endDraw(render);
        }
    }
}

void Maratis::drawMainView(Scene * scene)
{
    // get mouse
    MMouse * mouse = MMouse::getInstance();

    NeoEngine * engine = NeoEngine::getInstance();

    // get render
    RenderingContext * render = engine->getRenderingContext();
    Renderer * renderer = engine->getRenderer();

    // get camera
    OCamera * camera = getPerspectiveVue();
    Vector3 cameraAxis = camera->getRotatedVector(Vector3(0, 0, -1)).getNormalized();

    // clear z buffer
    render->enableDepthTest();
    render->clear(BUFFER_DEPTH);

    // draw grid
    camera->enable();
    //drawGrid(scene);

    scene->draw(camera);
    scene->drawObjectsBehaviors();

    camera->enable();

    // draw extra (box, triggers...)
    render->disableLighting();
    render->disableTexture();
    render->enableBlending();
    render->setBlendingMode(BLENDING_ALPHA);

    int * viewport = camera->getCurrentViewport();
    float editObjectSize = 10.0f / (float)viewport[3];

    // draw inactive / invisibles entities
    unsigned int i;
    unsigned int eSize = scene->getEntitiesNumber();
    for(i=0; i<eSize; i++)
    {
        OEntity * entity = scene->getEntityByIndex(i);
        if(! entity->isActive())
        {
            if(isObjectSelected(entity))
                render->setColor3(Vector3(1, 1, 0));
            else
                render->setColor3(Vector3(0.5f));

            entity->updateMatrix();
            drawInvisibleEntity(entity);
        }
        else if(entity->isInvisible())
        {
            if(isObjectSelected(entity))
                render->setColor3(Vector3(1, 1, 0));
            else
                render->setColor3(Vector3(1.0f));

            drawInvisibleEntity(entity);
        }
    }

    // draw sounds
    unsigned int sSize = scene->getSoundsNumber();
    for(i=0; i<sSize; i++)
    {
        OSound * sound = scene->getSoundByIndex(i);
        if(! sound->isActive())
            sound->updateMatrix();

        float size;
        if(! camera->isOrtho())
            size = (sound->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
        else
            size = camera->getFov() * editObjectSize;

        m_soundEntity->getMatrix()->loadIdentity();
        m_soundEntity->getMatrix()->setScale(sound->getTransformedScale() * Vector3(size));
        m_soundEntity->getMatrix()->setTranslationPart(sound->getTransformedPosition());

        if(isObjectSelected(sound))
            render->setColor3(Vector3(1, 1, 0));
        else
        {
            if(sound->isActive())
                render->setColor3(Vector3(1.0f));
            else
                render->setColor3(Vector3(0.5f));
        }

        render->pushMatrix();

        Matrix4x4 matrix = *camera->getMatrix();
        matrix.setTranslationPart(Vector3());

        render->multMatrix(m_soundEntity->getMatrix());
        render->multMatrix(&matrix);

        drawSound();

        render->popMatrix();
    }

    // draw lights
    unsigned int lSize = scene->getLightsNumber();
    for(i=0; i<lSize; i++)
    {
        OLight * light = scene->getLightByIndex(i);
        if(! light->isActive())
            light->updateMatrix();

        float size;
        if(! camera->isOrtho())
            size = (light->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
        else
            size = camera->getFov() * editObjectSize;

        m_lightEntity->getMatrix()->loadIdentity();
        m_lightEntity->getMatrix()->setScale(light->getTransformedScale() * Vector3(size));
        m_lightEntity->getMatrix()->setTranslationPart(light->getTransformedPosition());

        bool selected = isObjectSelected(light);
        if(selected)
            render->setColor3(Vector3(1, 1, 0));
        else
        {
            if(light->isActive())
                render->setColor3(Vector3(1.0f));
            else
                render->setColor3(Vector3(0.5f));
        }

        render->pushMatrix();

        Matrix4x4 matrix = *camera->getMatrix();
        matrix.setTranslationPart(Vector3(0, 0, 0));

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
                    render->setColor4(Vector4(1, 1, 0, 0.5f));
                else
                {
                    if(light->isActive())
                        render->setColor4(Vector4(1, 1, 1, 0.5f));
                    else
                        render->setColor4(Vector4(0.5f, 0.5f, 0.5f, 0.5f));
                }

                size = size*10;
                if(size > light->getRadius()*0.25f)
                    size = light->getRadius()*0.25f;

                Vector3 pos = light->getTransformedPosition();
                Vector3 dir1 = light->getRotatedVector(Vector3(0, 0, -1));//.getRotatedX(spotAngle)).getNormalized();
                Vector3 dir2 = light->getRotatedVector(Vector3(0, 0, -1));//.getRotatedX(-spotAngle)).getNormalized();

                Vector3 axis = light->getInverseRotatedVector(cameraAxis);
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

                beginDraw(PRIMITIVE_LINES);
                pushVertex(pos);
                pushVertex(pos + dir1*size);
                pushVertex(pos);
                pushVertex(pos + dir2*size);
                endDraw(render);

                beginDraw(PRIMITIVE_LINE_LOOP);
                for(int x=0; x<360; x+=10)
                {
                    Vector3 dir = Vector3(0, 0, -1).getRotatedX(spotAngle);
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
        OCamera * oCamera = scene->getCameraByIndex(i);
        if(! oCamera->isActive())
            oCamera->updateMatrix();

        if(! camera->isOrtho())
        {
            float scale = (oCamera->getTransformedPosition() - camera->getTransformedPosition()).getLength() * editObjectSize;
            Matrix4x4 scaleMatrix;
            scaleMatrix.setScale(Vector3(scale, scale, scale));
            (*m_cameraEntity->getMatrix()) = (*oCamera->getMatrix()) * scaleMatrix;
        }
        else
        {
            float scale = camera->getFov() * editObjectSize;
            Matrix4x4 scaleMatrix;
            scaleMatrix.setScale(Vector3(scale, scale, scale));
            (*m_cameraEntity->getMatrix()) = (*oCamera->getMatrix()) * scaleMatrix;
        }

        if(isObjectSelected(oCamera))
            render->setColor3(Vector3(1, 1, 0));
        else
        {
            if(oCamera->isActive())
                render->setColor3(Vector3(1.0f));
            else
                render->setColor3(Vector3(0.5f));
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
        OText * text = scene->getTextByIndex(i);
        if(! text->isActive())
            text->updateMatrix();

        Vector3 min = text->getBoundingBox()->min;
        Vector3 max = text->getBoundingBox()->max;

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
                render->setColor3(Vector3(1, 1, 0));
                drawBoundingBox(m_emptyText.getBoundingBox());
            }

            render->popMatrix();
        }
    }


    // draw armatures
    render->disableDepthTest();
    for(i=0; i<eSize; i++)
    {
        OEntity * entity = scene->getEntityByIndex(i);
        if(entity->isActive() && !entity->isInvisible() && isObjectSelected(entity))
        {
            render->setColor3(Vector3(0.75f));
            drawArmature(entity);
        }
    }
    render->enableDepthTest();

    // draw selected objects
    if((! mouse->isLeftButtonPushed()) || (m_currentAxis == M_AXIS_NONE))
    {
        render->enableBlending();
        render->setBlendingMode(BLENDING_ALPHA);

        Object3d * object;
        OEntity * entity;
        OText * text;

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
                    entity = (OEntity *)object;

                    render->setColor4(Vector4(1, 1, 0, 0.2f));
                    render->pushMatrix();
                    render->multMatrix(entity->getMatrix());
                    drawBoundingBox(entity->getBoundingBox());
                    render->popMatrix();
                }
                    break;
                case M_OBJECT3D_TEXT:
                {
                    text = (OText *)object;

                    render->setColor4(Vector4(1, 1, 0, 0.2f));
                    render->pushMatrix();
                    render->multMatrix(text->getMatrix());
                    drawBoundingBox(text->getBoundingBox());
                    render->popMatrix();
                }
            }
        }
    }

    drawGrid(engine->getLevel()->getCurrentScene());

    if(getSelectedObjectsNumber() > 0)
    {
        render->disableDepthTest();
        render->clear(BUFFER_DEPTH);
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

PROFILE_SHARED_DEFINE(Render);
PROFILE_SHARED_DEFINE(Update);

void Maratis::logicLoop(void)
{
    PROFILE_SHARED_BLOCK(Update);

    Maratis * maratis = Maratis::getInstance();
    NeoEngine * engine = NeoEngine::getInstance();
    Level * level = NeoEngine::getInstance()->getLevel();
    Scene * scene = level->getCurrentScene();

    // game
    NeoGame * game = engine->getGame();
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
        Object3d * object = scene->getObjectByIndex(i);
        if(! object->isActive())
            continue;

        object->updateBehaviors();
    }

    // update objects matrices
    scene->updateObjectsMatrices();
}

void Maratis::graphicLoop(void)
{
    PROFILE_SHARED_BLOCK(Render);
    RenderingContext * render = NeoEngine::getInstance()->getRenderingContext();

    NeoWindow* window = NeoWindow::getInstance();
    // viewport
    unsigned int w = (unsigned int)window->getWidth();
    unsigned int h = (unsigned int)window->getHeight();

    render->clear(BUFFER_COLOR | BUFFER_DEPTH);

    // game
    NeoGame * game = NeoEngine::getInstance()->getGame();
    if(game)
    {
        if(game->isRunning())
        {
            render->disableScissorTest();
            render->setViewport(0, 0, w, h);
            game->draw();
            return;
        }
    }

    //int x = (int)window->getPosition().x;
    //int y = window->getHeight() - h - (int)window->getPosition().y;
    render->setViewport(0, 0, w, h);

    // draw
    Maratis::getInstance()->drawMainView(NeoEngine::getInstance()->getLevel()->getCurrentScene());

    // setup render
    render->disableScissorTest();
    render->enableTexture();

    // 2d mode
    //set2dMode(render);
}
