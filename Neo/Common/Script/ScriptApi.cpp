//========================================================================
// Copyright (c) 2014-2015 Yannick Pflanzer <neo-engine.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <ScriptApi.h>
#include <Window/Window.h>

using namespace Neo;

char g_currentDirectory[256] = "";
unsigned long g_startTick = 0;

#define GET_OBJECT_SUBCLASS_BEGIN(type_, var_, type_enum)	\
	Object3d* object = (Object3d*) script->getPointer(0);	\
	if(object)	\
	{	\
		if(object->getType() == type_enum)	\
		{	\
			type_ * var_ = (type_*)object;

#define GET_OBJECT_SUBCLASS_END()	\
		}	\
	}

bool isFunctionOk(MScriptContext* script, const char* function, int argc)
{
	if(script->getArgsNumber() < argc)
	{
		MLOG_ERROR("'" << function << "' needs at least " << argc << " parameters!");
		return false;
	}

	return true;
}

bool getVector4(MScriptContext* script, unsigned int idx, MVector4* vec)
{
	if(script && vec)
	{
		script->getFloatArray(idx-1, reinterpret_cast<float*>(vec), 4);
		return true;
	}

	return false;
}

bool getVector3(MScriptContext* script, unsigned int idx, MVector3* vec)
{
	if(script && vec)
	{
		script->getFloatArray(idx-1, (float*) vec, 3);
		return true;
	}

	return false;
}

bool getVector2(MScriptContext* script, unsigned int idx, MVector2* vec)
{
	if(script && vec)
	{
		script->getFloatArray(idx-1, reinterpret_cast<float*>(vec), 2);
		return true;
	}

	return false;
}

static void linkObjects(Object3d *parent, Object3d *child)
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

static void unlinkObjects(Object3d *child)
{
	if(child == NULL)
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

static int vec3()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "vec", 3))
		return 0;

	float x = (float)script->getFloat(0);
	float y = (float)script->getFloat(1);
	float z = (float)script->getFloat(2);

	script->pushFloatArray(MVector3(x, y, z), 3);
	return 1;
}

static int length()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "length", 1))
		return 0;

	MVector3 a;
	script->getFloatArray(0, a, 3);
	script->pushFloat(a.getLength());
	return 1;
}

static int normalize()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "normalize", 1))
		return 0;

	MVector3 a;
	script->getFloatArray(0, a, 3);
	script->pushFloatArray(a.getNormalized(), 3);
	return 1;
}

static int dot()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "dot", 2))
		return 0;

	MVector3 a, b;
	script->getFloatArray(0, a, 3);
	script->getFloatArray(1, b, 3);
	script->pushFloat(a.dotProduct(b));
	return 1;
}

static int cross()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "cross", 2))
		return 0;

	MVector3 a, b;
	script->getFloatArray(0, a, 3);
	script->getFloatArray(1, b, 3);
	script->pushFloatArray(a.crossProduct(b), 3);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int getScene()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();

	if(! isFunctionOk(script, "getScene", 1))
		return 0;

	const char * name = script->getString(0);
	if(name)
	{
		unsigned int sceneIndex;
		if(level->getSceneIndexByName(name, &sceneIndex))
		{
			script->pushInteger(sceneIndex);
			return 1;
		}
	}

	MLOG_ERROR("Script: Scene \"" << name << "\" does not exist!");
	return 0;
}

int getCurrentCamera()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	int nbArguments = script->getArgsNumber();
	if(nbArguments == 1)
	{
		unsigned int sceneId = script->getInteger(0);
		scene = level->getSceneByIndex(sceneId);
	}

	OCamera * camera = scene->getCurrentCamera();
	if(camera)
	{
		script->pushPointer(camera);
		return 1;
	}

	return 0;
}

int getMeshFilename()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getMeshFilename", 1))
		return 0;

	int nbArguments = script->getArgsNumber()-1;

	Object3d* object = (Object3d*) script->getPointer(nbArguments);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity* entity = (OEntity*) object;
			script->pushString(entity->getMeshRef()->getFilename());
			return 1;
		}
	}

	MLOG_ERROR("Could not return mesh filename!");
	return 0;
}

// TODO: Encapsulasation
void getNewObjectName(const char * objectName, char * name)
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	unsigned int count = 0;
	int size = scene->getObjectsNumber();
	for(int i=0; i<size; i++)
	{
		Object3d * object = scene->getObjectByIndex(i);
		if(object->getName())
		{
			if(strcmp(name, object->getName()) == 0)
			{
				// name already exist
				count++;
				sprintf(name, "%s%d", objectName, count);
				i = -1;
			}
		}
	}
}

int loadMesh()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "loadMesh", 1))
		return 0;

	const char* path = script->getString(0);

	MeshRef* meshRef = NULL;
	Level* level = NeoEngine::getInstance()->getLevel();

	char string[256];
	getGlobalFilename(string, NeoWindow::getInstance()->getWorkingDirectory(), path);

	if(!isFileExist(string))
	{
		MLOG_ERROR("Could not load mesh file '" << path << "': File does not exist!");
		return 0;
	}

	meshRef = level->loadMesh(string, true);

	// create entity
	strcpy(string, "Entity0");
	OEntity * entity = level->getCurrentScene()->addNewEntity(meshRef);
	getNewObjectName("Entity", string);

	entity->setName(string);
	script->pushPointer(entity);

	return 1;
}

int loadSound()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "loadSound", 1))
		return 0;

	const char* path = script->getString(0);

	Level* level = NeoEngine::getInstance()->getLevel();
	Scene* scene = level->getCurrentScene();

	char string[256];
	getGlobalFilename(string, NeoWindow::getInstance()->getWorkingDirectory(), path);

	if(!isFileExist(string))
	{
		MLOG_ERROR("Could not load sound file '" << path << "': File does not exist!");
		return 0;
	}

	SoundRef* ref = level->loadSound(string);
	OSound* sound = scene->addNewSound(ref);

	strcpy(string, "Sound0");
	getNewObjectName("Sound", string);

	sound->setName(string);
	script->pushPointer(sound);

	return 1;
}

int getSoundFilename()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getSoundFilename", 1))
		return 0;

	int nbArguments = script->getArgsNumber();

	Object3d * object = (Object3d*) script->getPointer(nbArguments-1);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound* sound = (OSound*) object;
			script->pushString(sound->getSoundRef()->getFilename());
			return 1;
		}
	}

	MLOG_ERROR("Could not return sound filename!");
	return 0;
}

int getSoundRolloff()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getSoundRolloff", 1))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			script->pushInteger((long int)sound->getRolloff());
			return 1;
		}
	}

	return 0;
}

int setSoundRolloff()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setSoundRolloff", 2))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->setRolloff(script->getFloat(1));
		}
	}

	return 0;
}

int getSoundRadius()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getSoundRadius", 1))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			script->pushFloat(sound->getRadius());
			return 1;
		}
	}

	return 0;
}

int setSoundRadius()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setSoundRadius", 2))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->setRadius(script->getFloat(1));
		}
	}

	return 0;
}

int setSoundRelative()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setSoundRelative", 2))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->setRelative(script->getBoolean(1));
		}
	}

	return 1;
}

int isSoundRelative()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isSoundRelative", 1))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			script->pushBoolean(sound->isRelative());
			return 1;
		}
	}

	return 0;
}

int setSoundLooping()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setSoundLooping", 2))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->setLooping(script->getBoolean(1));
		}
	}

	return 0;
}

int isSoundLooping()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isSoundLooping", 1))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			script->pushBoolean(sound->isLooping());
			return 1;
		}
	}

	return 0;
}

int createGroup()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "createGroup", 0))
		return 0;

	Object3d* group = NeoEngine::getInstance()->getLevel()->getCurrentScene()->addNewGroup();
	char string[256] = "Group0";
	getNewObjectName("Group", string);
	group->setName(string);

	script->pushPointer(group);

	return 1;
}

int getObjectType()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getObjectType", 1))
		return 0;

	int nbArguments = script->getArgsNumber();
	Object3d* object = (Object3d*) script->getPointer(nbArguments-1);

	if(object)
	{
		switch (object->getType())
		{
		case M_OBJECT3D_ENTITY:
				script->pushString("Entity");
			break;
		case M_OBJECT3D_CAMERA:
				script->pushString("Camera");
			break;

		case M_OBJECT3D_LIGHT:
				script->pushString("Light");
			break;

		case M_OBJECT3D_SOUND:
				script->pushString("Sound");
			break;

		case M_OBJECT3D_TEXT:
				script->pushString("Text");
			break;

		case M_OBJECT3D:
				script->pushString("Object");
			break;
		default:
			break;
		}

		return 1;
	}

	return 0;
}

int getBoundingMin()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getBoundingMin", 1))
		return 0;

	int nbArguments = script->getArgsNumber();

	Object3d * object = (Object3d*) script->getPointer(nbArguments-1);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity* entity = (OEntity*) object;

			Mesh* mesh = entity->getMesh();
			script->pushFloatArray(mesh->getBoundingBox()->min, 3);

			return 1;
		}
	}

	return 0;
}

int getBoundingMax()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getBoundingMax", 1))
		return 0;

	int nbArguments = script->getArgsNumber();

	Object3d * object = (Object3d*) script->getPointer(nbArguments-1);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity* entity = (OEntity*) object;

			Mesh* mesh = entity->getMesh();
			script->pushFloatArray(mesh->getBoundingBox()->max, 3);

			return 1;
		}
	}

	return 0;
}

int deleteObject()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "deleteObject", 1))
		return 0;

	Level* level = NeoEngine::getInstance()->getLevel();
	Scene* scene;
	int nbArguments = script->getArgsNumber();
	if(nbArguments == 2)
	{
		unsigned int sceneId = script->getInteger(1);
		scene = level->getSceneByIndex(sceneId);
	}
	else
	{
		scene = level->getCurrentScene();
	}

	Object3d* object = (Object3d*) script->getPointer(0);
	if(object != NULL && object->getType() == M_OBJECT3D_ENTITY)
	{
		MPhysicsContext* physics = NeoEngine::getInstance()->getPhysicsContext();
		OEntity * entity = (OEntity*)object;
		PhysicsProperties * phyProps = entity->getPhysicsProperties();
		if(phyProps)
		{
			unsigned int id = phyProps->getCollisionObjectId();
			physics->deactivateObject(id);
			physics->deleteObject(&id);
		}
	}

	scene->deleteObject(object);
	return 1;
}

int getObject()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	if(! isFunctionOk(script, "getObject", 1))
		return 0;

	int nbArguments = script->getArgsNumber();
	if(nbArguments == 2)
	{
		unsigned int sceneId = script->getInteger(0);
		scene = level->getSceneByIndex(sceneId);
	}

	const char * name = script->getString(nbArguments-1);
	if(name)
	{
		Object3d * object = scene->getObjectByName(name);
		if(object)
		{
			script->pushPointer(object);
			return 1;
		}
	}

	MLOG_ERROR("Script: Object \"" << name << "\" does not exist!");
	return 0;
}

int objectExists()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	if(! isFunctionOk(script, "objectExists", 1))
		return 0;

	int nbArguments = script->getArgsNumber();
	if(nbArguments == 2)
	{
		unsigned int sceneId = script->getInteger(0);
		scene = level->getSceneByIndex(sceneId);
	}

	const char * name = script->getString(nbArguments-1);
	if(name)
	{
		Object3d * object = scene->getObjectByName(name);
		script->pushBoolean( object != NULL);
		return 1;
	}

	return 1;
}

int getClone()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();


	if(! isFunctionOk(script, "getClone", 1))
		return 0;

	int nbArguments = script->getArgsNumber();
	if(nbArguments == 2)
	{
		unsigned int sceneId = script->getInteger(0);
		scene = level->getSceneByIndex(sceneId);
	}

	Object3d * object = (Object3d*) script->getPointer(nbArguments-1);
	if(object)
	{
		Object3d * cloneObj = NULL;

		switch(object->getType())
		{
			case M_OBJECT3D_CAMERA:
				cloneObj = scene->addNewCamera(*(OCamera*)object);
				break;
			case M_OBJECT3D_LIGHT:
				cloneObj = scene->addNewLight(*(OLight*)object);
				break;
			case M_OBJECT3D_ENTITY:
				cloneObj = scene->addNewEntity(*(OEntity*)object);
				scene->prepareCollisionObject((OEntity*)cloneObj);
				break;
			case M_OBJECT3D_SOUND:
				cloneObj = scene->addNewSound(*(OSound*)object);
				break;
			case M_OBJECT3D_TEXT:
				cloneObj = scene->addNewText(*(OText*)object);
				break;
		}

		if(cloneObj)
		{
			char name[256];
			sprintf(name, "%s_clone%d", object->getName(), scene->getObjectsNumber());
			cloneObj->setName(name);
		}

		script->pushPointer(cloneObj);
		return 1;
	}

	return 0;
}

int getParent()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getParent", 1))
		return 0;

	Object3d * object = (Object3d*) script->getPointer(0);
	if(object)
	{
		script->pushPointer(object->getParent());
		return 1;
	}

	return 0;
}

int getChilds()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getChilds", 1))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);

	if(object)
	{
		unsigned int cSize = object->getChildsNumber();
		long int * childs = new long int[cSize];

		for(unsigned int c=0; c<cSize; c++)
			childs[c] = (long int)object->getChild(c);

		script->pushIntArray((int*)childs, cSize);

		delete [] childs;
		return 1;
	}

	return 0;
}

int getProjectedPoint()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getProjectedPoint", 2))
		return 0;

	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	unsigned int width = 0;
	unsigned int height = 0;
	system->getScreenSize(&width, &height);

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MVector3 vec;
			if(getVector3(script, 2, &vec))
			{
				OCamera * camera = (OCamera *)object;
				MVector3 result = camera->getProjectedPoint(vec);
				result.x = result.x/(float)width;
				result.y = 1.0f - (result.y/(float)height);
				script->pushFloatArray( result, 3);
				return 1;
			}
		}
	}

	return 0;
}

int getUnProjectedPoint()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getUnProjectedPoint", 2))
		return 0;

	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	unsigned int width = 0;
	unsigned int height = 0;
	system->getScreenSize(&width, &height);

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MVector3 vec;
			if(getVector3(script, 2, &vec))
			{
				OCamera * camera = (OCamera *)object;
				MVector3 result = camera->getUnProjectedPoint(MVector3(vec.x*width, (1-vec.y)*height, vec.z));
				script->pushFloatArray( result, 3);
				return 1;
			}
		}
	}

	return 0;
}

int rotate()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "rotate", 3))
		return 0;

	int nbArguments = script->getArgsNumber();

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 axis;
		if(getVector3(script, 2, &axis))
		{
			// get angle
			float angle = (float)script->getFloat(2);

			// is local ?
			bool local = false;
			if(nbArguments > 3)
			{
				const char * mode = script->getString(3);
				if(mode && strcmp(mode, "local") == 0)
					local = true;
			}

			// rotate
			if(local)
				object->addAxisAngleRotation(axis, angle);
			else
				object->addAxisAngleRotation(object->getInverseRotatedVector(axis), angle);
		}
	}

	return 0;
}

int translate()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "translate", 2))
		return 0;

	int nbArguments = script->getArgsNumber();

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 axis;
		if(getVector3(script, 2, &axis))
		{
			// is local ?
			bool local = false;
			if(nbArguments > 2)
			{
				const char * mode = script->getString(2);
				if(mode && strcmp(mode, "local") == 0)
					local = true;
			}

			// rotate
			if(local)
			{
				axis = object->getRotatedVector(axis);
				object->setPosition(object->getPosition() + axis);
			}
			else
				object->setPosition(object->getPosition() + axis);
		}
	}

	return 0;
}

int getPosition()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getPosition", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushFloatArray(object->getPosition(), 3);
		return 1;
	}

	return 0;
}

int getRotation()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getRotation", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushFloatArray( object->getEulerRotation(), 3);
		return 1;
	}

	return 0;
}

int getScale()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getScale", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushFloatArray( object->getScale(), 3);
		return 1;
	}

	return 0;
}

int getTransformedPosition()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getTransformedPosition", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushFloatArray( object->getTransformedPosition(), 3);
		return 1;
	}

	return 0;
}

int getTransformedRotation()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getTransformedRotation", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushFloatArray( object->getTransformedRotation(), 3);
		return 1;
	}

	return 0;
}

int getTransformedScale()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getTransformedScale", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushFloatArray( object->getTransformedScale(), 3);
		return 1;
	}

	return 0;
}

int setPosition()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setPosition", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 position;
		if(getVector3(script, 2, &position))
		{
			object->setPosition(position);
		}
	}

	return 0;
}

int setRotation()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setRotation", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 rotation;
		if(getVector3(script, 2, &rotation))
		{
			object->setEulerRotation(rotation);
		}
	}

	return 0;
}

int setScale()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setScale", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 scale;
		if(getVector3(script, 2, &scale))
		{
			object->setScale(scale);
		}
	}

	return 0;
}

int updateMatrix()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "updateMatrix", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		object->updateMatrix();
	}

	return 0;
}

int getMatrix()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getMatrix", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushFloatArray( object->getMatrix()->entries, 16);
		return 1;
	}

	return 0;
}

int getInverseRotatedVector()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getInverseRotatedVector", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 vec;
		if(getVector3(script, 2, &vec))
		{
			script->pushFloatArray( object->getInverseRotatedVector(vec), 3);
			return 1;
		}
	}

	return 0;
}

int getRotatedVector()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getRotatedVector", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 vec;
		if(getVector3(script, 2, &vec))
		{
			script->pushFloatArray( object->getRotatedVector(vec), 3);
			return 1;
		}
	}

	return 0;
}

int getInverseVector()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getInverseVector", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 vec;
		if(getVector3(script, 2, &vec))
		{
			script->pushFloatArray( object->getInversePosition(vec), 3);
			return 1;
		}
	}

	return 0;
}

int getTransformedVector()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getTransformedVector", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 vec;
		if(getVector3(script, 2, &vec))
		{
			script->pushFloatArray( object->getTransformedVector(vec), 3);
			return 1;
		}
	}

	return 0;
}

int isVisible()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isVisible", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		int vis = object->isVisible();
		script->pushBoolean( vis);
		return 1;
	}

	return 0;
}

int setInvisible()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setInvisible", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity* entity = static_cast<OEntity*>(object);
			entity->setInvisible(script->getBoolean(1) != 0);
		}
		return 1;
	}

	return 0;
}

int activate()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "activate", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(! object->isActive())
		{
			object->setActive(true);
			if(object->getType() == M_OBJECT3D_ENTITY)
			{
				OEntity * entity = (OEntity*)object;
				PhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
					physics->activateObject(phyProps->getCollisionObjectId());
			}
		}
	}

	return 0;
}

int deactivate()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "deactivate", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->isActive())
		{
			object->setActive(false);
			if(object->getType() == M_OBJECT3D_ENTITY)
			{
				OEntity * entity = (OEntity*)object;
				PhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
					physics->deactivateObject(phyProps->getCollisionObjectId());
			}
		}
	}

	return 0;
}

int isActive()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isActive", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		script->pushBoolean( object->isActive());
		return 1;
	}

	return 0;
}

int getName()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getName", 1))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);
	if(object)
	{
		script->pushString(object->getName());
		return 1;
	}

	return 0;
}

int setParent()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setParent", 2))
		return 0;

	Object3d * object  = (Object3d*) script->getPointer(0);
	if(object)
	{
		Object3d * parent = (Object3d*) script->getPointer(1);
		if(parent)
			linkObjects(parent, object);
		else
			unlinkObjects(object);

		object->updateMatrix();
		return 0;
	}

	return 0;
}

int changeAnimation()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "changeAnimation", 2))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;

			unsigned int anim = (unsigned int)script->getInteger(1);
			entity->changeAnimation(anim);
			return 0;
		}
	}

	return 0;
}

int isAnimationOver()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isAnimationOver", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			int isAnimOver = entity->isAnimationOver();
			script->pushBoolean( isAnimOver);
			return 1;
		}
	}

	return 0;
}

int getCurrentAnimation()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getCurrentAnimation", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			int anim = (int)entity->getAnimationId();
			script->pushInteger( anim);
			return 1;
		}
	}

	return 0;
}

int setAnimationSpeed()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setAnimationSpeed", 2))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OEntity, entity, M_OBJECT3D_ENTITY)
			entity->setAnimationSpeed((float)script->getFloat(1));
			return 0;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int getAnimationSpeed()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getAnimationSpeed", 1))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OEntity, entity, M_OBJECT3D_ENTITY)
			script->pushFloat( (float)entity->getAnimationSpeed());
			return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int setCurrentFrame()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setCurrentFrame", 2))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OEntity, entity, M_OBJECT3D_ENTITY)
			entity->setCurrentFrame((float)script->getFloat(1));
			return 0;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int getCurrentFrame()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getCurrentFrame", 1))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OEntity, entity, M_OBJECT3D_ENTITY)
			script->pushFloat( (float)entity->getCurrentFrame());
			return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int getGravity()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	int nbArguments = script->getArgsNumber();
	if(nbArguments == 1)
	{
		unsigned int sceneId = script->getInteger(0);
		scene = level->getSceneByIndex(sceneId);
	}

	script->pushFloatArray( scene->getGravity(), 3);

	return 1;
}

int setGravity()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	if(! isFunctionOk(script, "setGravity", 1))
		return 0;

	int nbArguments = script->getArgsNumber();
	if(nbArguments == 2)
	{
		unsigned int sceneId = script->getInteger(0);
		scene = level->getSceneByIndex(sceneId);
	}

	MVector3 gravity;
	if(getVector3(script, nbArguments, &gravity))
		scene->setGravity(gravity);

	return 0;
}

int changeCurrentCamera()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	if(! isFunctionOk(script, "changeCurrentCamera", 1))
		return 0;

	int nbArguments = script->getArgsNumber();
	if(nbArguments == 2)
	{
		unsigned int sceneId = script->getInteger(0);
		scene = level->getSceneByIndex(sceneId);
	}

	Object3d * object = (Object3d*) script->getPointer(nbArguments-1);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			unsigned int i;
			unsigned int size = scene->getCamerasNumber();
			for(i=0; i<size; i++)
			{
				if(object == (Object3d *)scene->getCameraByIndex(i))
				{
					scene->setCurrentCameraId(i);
					break;
				}
			}
		}
	}

	return 0;
}

int isGhost()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isGhost", 1))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OEntity, entity, M_OBJECT3D_ENTITY)
			PhysicsProperties* phys = entity->getPhysicsProperties();

			if(!phys)
			{
				script->pushBoolean( false);
				return 1;
			}

			script->pushBoolean( phys->isGhost());
			return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int enableGhost()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "enableGhost", 2))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OEntity, entity, M_OBJECT3D_ENTITY)
			PhysicsProperties* phys = entity->getPhysicsProperties();

			if(!phys)
			{
				return 0;
			}

			phys->setGhost(script->getBoolean(1));
			return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int addCentralForce()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	int nbArguments = script->getArgsNumber();
	if(! isFunctionOk(script, "addCentralForce", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MVector3 force;
			if(getVector3(script, 2, &force))
			{
				// is local ?
				bool local = false;
				if(nbArguments > 2)
				{
					const char * mode = script->getString(2);
					if(mode && strcmp(mode, "local") == 0)
						local = true;
				}

				OEntity * entity = (OEntity*)object;
				PhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
				{
					if(local)
						force = object->getRotatedVector(force);

					MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
					physics->addCentralForce(phyProps->getCollisionObjectId(), force);
				}
			}
		}
	}

	return 0;
}

int clearForces()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
				physics->clearForces(phyProps->getCollisionObjectId());
			}
		}
	}

	return 0;
}

int addTorque()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	int nbArguments = script->getArgsNumber();
	if(! isFunctionOk(script, "addTorque", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MVector3 torque;
			if(getVector3(script, 2, &torque))
			{
				// is local ?
				bool local = false;
				if(nbArguments > 2)
				{
					const char * mode = script->getString(2);
					if(mode && strcmp(mode, "local") == 0)
						local = true;
				}

				OEntity * entity = (OEntity*)object;
				PhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
				{
					if(local)
						torque = object->getRotatedVector(torque);

					MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
					physics->addTorque(phyProps->getCollisionObjectId(), torque);
				}
			}
		}
	}

	return 0;
}

int getLinearDamping()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getLinearDamping", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float linearDamping = phyProps->getLinearDamping();
				script->pushFloat(linearDamping);
				return 1;
			}
		}
	}

	return 0;
}

int setLinearDamping()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "setLinearDamping", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float linearDamping = (float)script->getFloat(1);
				phyProps->setLinearDamping(linearDamping);
				physics->setObjectDamping(
					phyProps->getCollisionObjectId(),
					phyProps->getLinearDamping(),
					phyProps->getAngularDamping()
					);
				return 0;
			}
		}
	}

	return 0;
}

int getAngularDamping()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getAngularDamping", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float angularDamping = phyProps->getAngularDamping();
				script->pushFloat( (float)angularDamping);
				return 1;
			}
		}
	}

	return 0;
}

int setAngularDamping()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "setAngularDamping", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float angularDamping = (float)script->getFloat(1);
				phyProps->setAngularDamping(angularDamping);
				physics->setObjectDamping(
					phyProps->getCollisionObjectId(),
					phyProps->getLinearDamping(),
					phyProps->getAngularDamping()
					);
				return 0;
			}
		}
	}

	return 0;
}

int setConstraintParent()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setConstraintParent", 2))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);
	Object3d* parent = (Object3d*) script->getPointer(1);
	if(object && parent)
	{
		if(object->getType() == M_OBJECT3D_ENTITY && parent->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				PhysicsConstraint* constraint = phyProps->getConstraint();

				if(constraint)
				{
					constraint->parentName.set(parent->getName());
					NeoEngine::getInstance()->getLevel()->getCurrentScene()->prepareConstraints(entity);
				}
				else
				{
					MLOG_ERROR("script: Can not set constraint parent: No constraint available!");
				}
				return 0;
			}
		}
	}
}

int getConstraintParent()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setConstraintParent", 2))
		return 0;

	Object3d* object;
	Object3d* parent;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				PhysicsConstraint* constraint = phyProps->getConstraint();

				if(constraint)
				{
					Object3d* object = NeoEngine::getInstance()->getLevel()->getCurrentScene()->getObjectByName(constraint->parentName.getSafeString());
					script->pushInteger( (long int) object);
				}
				else
				{
					MLOG_ERROR("script: Can not get constraint parent: No constraint available!");
				}
				return 0;
			}
		}
	}
}

int enableParentCollision()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "enableParentCollision", 2))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);
	bool collision = script->getBoolean(1);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				PhysicsConstraint* constraint = phyProps->getConstraint();

				if(constraint)
				{
				   constraint->disableParentCollision = !collision;
				}
				else
				{
					MLOG_ERROR("script: Can not set constraint parent: No constraint available!");
				}
				return 0;
			}
		}
	}
}

int getCentralForce()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "getCentralForce", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 force;
				physics->getCentralForce(phyProps->getCollisionObjectId(), &force);
				script->pushFloatArray( force, 3);
				return 1;
			}
		}
	}

	return 0;
}

int getTorque()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "getTorque", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 force;
				physics->getTorque(phyProps->getCollisionObjectId(), &force);
				script->pushFloatArray( force, 3);
				return 1;
			}
		}
	}

	return 0;
}

int getMass()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getMass", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float mass = phyProps->getMass();
				script->pushFloat( (float)mass);
				return 1;
			}
		}
	}

	return 0;
}

int enablePhysics()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if (!isFunctionOk(script, "enablePhysics", 2))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OEntity, entity, M_OBJECT3D_ENTITY)
	MPhysicsContext* physics = NeoEngine::getInstance()->getPhysicsContext();
	PhysicsProperties* phyProps = entity->createPhysicsProperties();
	Scene* scene = NeoEngine::getInstance()->getLevel()->getCurrentScene();

	if (script->getBoolean(0) != 0)
	{
		entity->deletePhysicsProperties();
		return 1;
	}

	scene->prepareCollisionObject(entity);

	return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

int setMass()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "setMass", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float mass = (float)script->getFloat(1);
				phyProps->setMass(mass);
				physics->setObjectMass(phyProps->getCollisionObjectId(), mass);
				return 0;
			}
		}
	}

	return 0;
}

int getFriction()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getFriction", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float friction = phyProps->getFriction();
				script->pushFloat( (float)friction);
				return 1;
			}
		}
	}

	return 0;
}

int setFriction()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "setFriction", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float friction = (float)script->getFloat(1);
				phyProps->setFriction(friction);
				physics->setObjectFriction(phyProps->getCollisionObjectId(), friction);
				return 0;
			}
		}
	}

	return 0;
}

int getRestitution()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getRestitution", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float restitution = phyProps->getRestitution();
				script->pushFloat( (float)restitution);
				return 1;
			}
		}
	}

	return 0;
}

int setRestitution()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "setRestitution", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float restitution = (float)script->getFloat(1);
				phyProps->setRestitution(restitution);
				physics->setObjectRestitution(phyProps->getCollisionObjectId(), restitution);
				return 0;
			}
		}
	}

	return 0;
}

int getAngularFactor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getAngularFactor", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float aFactor = phyProps->getAngularFactor();
				script->pushFloat( (float)aFactor);
				return 1;
			}
		}
	}

	return 0;
}

int setAngularFactor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "setAngularFactor", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float aFactor = (float)script->getFloat(1);
				phyProps->setAngularFactor(aFactor);
				physics->setObjectAngularFactor(phyProps->getCollisionObjectId(), aFactor);
				return 0;
			}
		}
	}

	return 0;
}

int getLinearFactor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getLinearFactor", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 * linFactor = phyProps->getLinearFactor();
				script->pushFloatArray( *linFactor, 3);
				return 1;
			}
		}
	}

	return 0;
}

int setLinearFactor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(script, "setLinearFactor", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MVector3 linFactor;
				if(getVector3(script, 2, &linFactor))
				{
					phyProps->setLinearFactor(linFactor);
					physics->setObjectLinearFactor(phyProps->getCollisionObjectId(), linFactor);
					return 0;
				}
			}
		}
	}

	return 0;
}

int getNumCollisions()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getNumCollisions", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
				int collision = physics->isObjectInCollision(phyProps->getCollisionObjectId());
				script->pushInteger( collision);
				return 1;
			}
		}
	}

	return 0;
}

int isCollisionTest()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isCollisionTest", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			OEntity * entity = (OEntity*)object;
			PhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
				int collision = MIN(1, physics->isObjectInCollision(phyProps->getCollisionObjectId()));
				script->pushBoolean( collision);
				return 1;
			}
		}
	}

	return 0;
}

int isCollisionBetween()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isCollisionBetween", 2))
		return 0;

	Object3d * object1;
	Object3d * object2;

	object1 = (Object3d*) script->getPointer(0);
	object2 = (Object3d*) script->getPointer(1);
	if(object1 && object2)
	{
		if((object1->getType() == M_OBJECT3D_ENTITY) && (object2->getType() == M_OBJECT3D_ENTITY))
		{
			OEntity * entity1 = (OEntity*)object1;
			OEntity * entity2 = (OEntity*)object2;

			PhysicsProperties * phyProps1 = entity1->getPhysicsProperties();
			PhysicsProperties * phyProps2 = entity2->getPhysicsProperties();
			if(phyProps1 && phyProps2)
			{
				MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
				int collision = physics->isObjectsCollision(
					phyProps1->getCollisionObjectId(),
					phyProps2->getCollisionObjectId()
					);
				script->pushBoolean( collision);
				return 1;
			}
		}
	}

	return 0;
}

int rayHit()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Level * level = NeoEngine::getInstance()->getLevel();
	Scene * scene = level->getCurrentScene();

	if(! isFunctionOk(script, "rayHit", 2))
		return 0;

	int nbArguments = script->getArgsNumber();

	MVector3 start, end;
	if(getVector3(script, 1, &start) && getVector3(script, 2, &end))
	{
		MPhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

		unsigned int collisionObjId;
		MVector3 point;

		// ray test
		if(physics->isRayHit(start, end, &collisionObjId, &point))
		{
			if(nbArguments == 3)
			{
				Object3d * object = (Object3d*) script->getPointer(2);
				if(object)
				{
					if(object->getType() == M_OBJECT3D_ENTITY)
					{
						OEntity * entity = (OEntity*)object;
						PhysicsProperties * phyProps = entity->getPhysicsProperties();
						if(phyProps)
						{
							if(phyProps->getCollisionObjectId() == collisionObjId)
							{
								script->pushFloatArray( point, 3);
								return 1;
							}
						}
					}
				}
			}
			else
			{
				script->pushFloatArray( point, 3);

				unsigned int e, eSize = scene->getEntitiesNumber();
				for(e=0; e<eSize; e++)
				{
					OEntity * entity = scene->getEntityByIndex(e);
					PhysicsProperties * phyProps = entity->getPhysicsProperties();
					if(phyProps)
					{
						if(phyProps->getCollisionObjectId() == collisionObjId)
						{
							script->pushInteger( (long int)entity);
							return 2;
						}
					}
				}

				return 1;
			}
		}
	}

	return 0;
}

int isKeyPressed()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if(! isFunctionOk(script, "isKeyPressed", 1))
		return 0;

	const char * name = script->getString(0);
	if(name)
	{
		int keyPressed = input->isKeyPressed(name);
		script->pushBoolean( keyPressed);
		return 1;
	}

	return 0;
}

int onKeyDown()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if(! isFunctionOk(script, "onKeyDown", 1))
		return 0;

	const char * name = script->getString(0);
	if(name)
	{
		int keyDown = input->onKeyDown(name);
		script->pushBoolean( keyDown);
		return 1;
	}

	return 0;
}

int onKeyUp()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if(! isFunctionOk(script, "onKeyUp", 1))
		return 0;

	const char * name = script->getString(0);
	if(name)
	{
		int keyUp = input->onKeyUp(name);
		script->pushBoolean( keyUp);
		return 1;
	}

	return 0;
}

int getAxis()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if(! isFunctionOk(script, "getAxis", 1))
		return 0;

	const char * name = script->getString(0);
	if(name)
	{
		float axis = input->getAxis(name);
		script->pushFloat( axis);
		return 1;
	}

	return 0;
}

int getProperty()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if(! isFunctionOk(script, "getProperty", 1))
		return 0;

	const char * name = script->getString(0);
	if(name)
	{
		int prop = input->getProperty(name);
		script->pushInteger( prop);
		return 1;
	}

	return 0;
}

int getTouchPosition()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if (!isFunctionOk(script, "getTouchPosition", 1))
		return 0;

	unsigned int touchID = (unsigned int)script->getInteger(0);
	script->pushFloatArray( input->getTouchPosition(touchID), 2);

	return 1;
}

int getLastTouchPosition()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if (!isFunctionOk(script, "getLastTouchPosition", 1))
		return 0;

	unsigned int touchID = (unsigned int)script->getInteger(0);
	script->pushFloatArray( input->getLastTouchPosition(touchID), 2);

	return 1;
}

int getTouchPhase()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	MInputContext * input = NeoEngine::getInstance()->getInputContext();

	if (!isFunctionOk(script, "getTouchPhase", 1))
		return 0;

	unsigned int touchID = (unsigned int)script->getInteger(0);
	script->pushInteger( input->getTouchPhase(touchID));

	return 1;
}

int playSound()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "playSound", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->play();
		}
	}

	return 0;
}

int pauseSound()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "pauseSound", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->pause();
		}
	}

	return 0;
}

int stopSound()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "stopSound", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->stop();
		}
	}

	return 0;
}

int setSoundPitch()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setSoundPitch", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			sound->setPitch(script->getFloat(1));
		}
	}

	return 0;
}

int getSoundPitch()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "getSoundPitch", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			script->pushFloat( sound->getPitch());
		}
	}

	return 1;
}

int changeScene()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoEngine * engine = NeoEngine::getInstance();
	Level * level = engine->getLevel();

	if(! isFunctionOk(script, "changeScene", 1))
		return 0;

	long int id = script->getInteger(0);
	level->changeCurrentScene(id);

	return 0;
}

int getCurrentSceneId()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoEngine * engine = NeoEngine::getInstance();
	Level * level = engine->getLevel();

	script->pushInteger( (int)level->getCurrentSceneId());
	return 1;
}

int getScenesNumber()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoEngine * engine = NeoEngine::getInstance();
	Level * level = engine->getLevel();

	script->pushInteger( level->getScenesNumber());
	return 1;
}

int loadLevel()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "loadLevel", 1))
		return 0;

	const char * filename = script->getString(0);
	if(filename)
		NeoEngine::getInstance()->requestLoadLevel(filename);

	return 0;
}

int quit()
{
	NeoEngine * engine = NeoEngine::getInstance();
	engine->setActive(false);
	return 0;
}

int doesLevelExist()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoEngine * engine = NeoEngine::getInstance();

	if(! isFunctionOk(script, "doesLevelExist", 1))
		return 0;

	const char * filename = script->getString(0);

	script->pushBoolean(engine->doesLevelExist(filename));
	return 1;
}

int getLightColor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getLightColor", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*) object;
			script->pushFloatArray(light->getColor(), 3);
			return 1;
		}
	}

	return 0;
}

int getLightRadius()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getLightRadius", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushFloat( (float)light->getRadius());
			return 1;
		}
	}

	return 0;
}

int getLightIntensity()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getLightIntensity", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushFloat( (float)light->getIntensity());
			return 1;
		}
	}

	return 0;
}

int setLightColor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setLightColor", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		MVector3 color;
		if(object->getType() == M_OBJECT3D_LIGHT && getVector3(script, 2, &color))
		{
			OLight * light = (OLight*)object;
			light->setColor(color);
			return 0;
		}
	}

	return 0;
}

int setLightRadius()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setLightRadius", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float radius = (float)script->getFloat(1);

			OLight * light = (OLight*)object;
			light->setRadius(radius);
			return 0;
		}
	}

	return 0;
}

int setLightIntensity()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setLightIntensity", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float intensity = (float)script->getFloat(1);

			OLight * light = (OLight*)object;
			light->setIntensity(intensity);
			return 0;
		}
	}

	return 0;
}

int enableShadow()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "enableShadow", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			bool shadow = script->getBoolean(1) == 1;
			OLight* light = (OLight*) object;
			light->castShadow(shadow);
			return 0;
		}
		else if(object->getType() == M_OBJECT3D_ENTITY)
		{
			bool shadow = script->getBoolean(1) == 1;
			OEntity* entity = (OEntity*) object;
			entity->enableShadow(shadow);
			return 0;
		}
	}

	return 0;
}

int isCastingShadow()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isCastingShadow", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushBoolean( light->isCastingShadow());
			return 1;
		}
	}

	return 0;
}

int setlightShadowQuality()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setlightShadowQuality", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			int quality = script->getInteger(1);
			OLight * light = (OLight*)object;
			light->setShadowQuality(quality);
			return 0;
		}
	}

	return 0;
}

int setlightShadowBias()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setlightShadowBias", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float bias = script->getFloat(1);
			OLight * light = (OLight*)object;
			light->setShadowBias(bias);
			return 0;
		}
	}

	return 0;
}

int setlightShadowBlur()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setlightShadowBlur", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float blur = script->getFloat(1);
			OLight * light = (OLight*)object;
			light->setShadowBlur(blur);
			return 0;
		}
	}

	return 0;
}

int getlightShadowQuality()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getlightShadowQuality", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushInteger( light->getShadowQuality());
			return 1;
		}
	}

	return 0;
}

int getlightShadowBias()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getlightShadowBias", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushFloat( light->getShadowBias());
			return 1;
		}
	}

	return 0;
}

int getlightShadowBlur()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getlightShadowBlur", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushFloat( light->getShadowBlur());
			return 1;
		}
	}

	return 0;
}

int setlightSpotAngle()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setlightSpotAngle", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float angle = script->getFloat(1);
			OLight * light = (OLight*)object;
			light->setSpotAngle(angle);
			return 0;
		}
	}

	return 0;
}

int setlightSpotExponent()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setlightSpotExponent", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float exponent = script->getFloat(1);
			OLight * light = (OLight*)object;
			light->setSpotExponent(exponent);
			return 0;
		}
	}

	return 0;
}

int getlightSpotAngle()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getlightSpotAngle", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushFloat( light->getSpotAngle());
			return 1;
		}
	}

	return 0;
}

int getlightSpotExponent()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getlightSpotExponent", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			OLight * light = (OLight*)object;
			script->pushFloat( light->getSpotExponent());
			return 1;
		}
	}

	return 0;
}

int createLight()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	OLight* light = NeoEngine::getInstance()->getLevel()->getCurrentScene()->addNewLight();
	char name[256] = "Light0";
	getNewObjectName("Light", name);

	light->setName(name);
	script->pushInteger( (long int) light);

	return 1;
}

int getSoundGain()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getSoundGain", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			OSound * sound = (OSound*)object;
			script->pushFloat( (float)sound->getGain());
			return 1;
		}
	}

	return 0;
}

int setSoundGain()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setSoundGain", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			float gain = (float)script->getFloat(1);
			OSound * sound = (OSound*)object;
			sound->setGain(gain);
			return 0;
		}
	}

	return 0;
}

int createCamera()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	OCamera* camera = NeoEngine::getInstance()->getLevel()->getCurrentScene()->addNewCamera();
	char name[256] = "Camera0";
	getNewObjectName("Camera", name);

	camera->setName(name);
	script->pushPointer(camera);
	return 1;
}

int setCameraClearColor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setCameraClearColor", 2))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);

	if(object)
	{
		MVector3 color;
		if(object->getType() == M_OBJECT3D_CAMERA && getVector3(script, 2, &color))
		{
			OCamera * camera = (OCamera*)object;
			camera->setClearColor(color);
			return 0;
		}
	}

	return 0;
}

int getCameraClearColor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getCameraClearColor", 1))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			script->pushFloatArray( camera->getClearColor(), 3);
			return 1;
		}
	}

	return 0;
}

int setCameraNear()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setCameraNear", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float clipNear = (float)script->getFloat(1);
			OCamera * camera = (OCamera*)object;
			camera->setClippingNear(clipNear);
			return 0;
		}
	}

	return 0;
}

int getCameraNear()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getCameraNear", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			script->pushFloat( (float)camera->getClippingNear());
			return 1;
		}
	}

	return 0;
}

int setCameraFar()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setCameraFar", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float clipFar = (float)script->getFloat(1);
			OCamera * camera = (OCamera*)object;
			camera->setClippingFar(clipFar);
			return 0;
		}
	}

	return 0;
}

int getCameraFar()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getCameraFar", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			script->pushFloat( (float)camera->getClippingFar());
			return 1;
		}
	}

	return 0;
}

int setCameraFov()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setCameraFov", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float fov = (float)script->getFloat(1);
			OCamera * camera = (OCamera*)object;
			camera->setFov(fov);
			return 0;
		}
	}

	return 0;
}

int getCameraFov()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getCameraFov", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			script->pushFloat( (float)camera->getFov());
			return 1;
		}
	}

	return 0;
}

int setCameraFogDistance()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setCameraFogDistance", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float fogDist = (float)script->getFloat(1);
			OCamera * camera = (OCamera*)object;
			camera->setFogDistance(fogDist);
			return 0;
		}
	}

	return 0;
}

int getCameraFogDistance()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getCameraFogDistance", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			script->pushFloat( (float)camera->getFogDistance());
			return 1;
		}
	}

	return 0;
}

int enableCameraOrtho()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "enableCameraOrtho", 2))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			bool ortho = script->getBoolean(1) == 1;
			OCamera * camera = (OCamera*)object;
			camera->enableOrtho(ortho);
			return 0;
		}
	}

	return 0;
}

int isCameraOrtho()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isCameraOrtho", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			script->pushBoolean( camera->isOrtho());
			return 1;
		}
	}

	return 0;
}

int enableCameraFog()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "enableCameraFog", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			bool fog = script->getBoolean(1) == 1;
			OCamera * camera = (OCamera*)object;
			camera->enableFog(fog);
			return 0;
		}
	}

	return 0;
}

int isCameraFogEnabled()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "isCameraFogEnabled", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			script->pushBoolean( camera->hasFog());
			return 1;
		}
	}

	return 0;
}

int enableCameraLayer()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "enableCameraLayer", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			unsigned int sceneId = (unsigned int)script->getInteger(1);
			OCamera * camera = (OCamera*)object;
			camera->setSceneLayer(sceneId+1);
			return 0;
		}
	}

	return 0;
}

int disableCameraLayer()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "disableCameraLayer", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera*)object;
			camera->setSceneLayer(0);
			return 0;
		}
	}

	return 0;
}

int enableRenderToTexture()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	MRenderingContext * render = engine->getRenderingContext();
	Level * level = engine->getLevel();

	if(! isFunctionOk(script, "enableRenderToTexture", 4))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			const char * name = script->getString(1);
			if(name)
			{
				OCamera * camera = (OCamera *)object;

				unsigned int width = (unsigned int)script->getInteger(2);
				unsigned int height = (unsigned int)script->getInteger(3);

				char globalFilename[256];
				getGlobalFilename(globalFilename, system->getWorkingDirectory(), name);

				TextureRef * colorTexture = level->loadTexture(globalFilename, 0, 0);
				TextureRef * depthTexture = level->loadTexture(object->getName(), 0, 0);

				colorTexture->clear();
				depthTexture->clear();

				camera->setRenderColorTexture(colorTexture);
				camera->setRenderDepthTexture(depthTexture);

				unsigned int m_colorTextureId, m_depthTextureId;

				render->createTexture(&m_colorTextureId);
				render->bindTexture(m_colorTextureId);
				render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
				render->setTextureUWrapMode(M_WRAP_CLAMP);
				render->setTextureVWrapMode(M_WRAP_CLAMP);
				render->texImage(0, width, height, M_UBYTE, M_RGBA, 0);

				render->createTexture(&m_depthTextureId);
				render->bindTexture(m_depthTextureId);
				render->setTextureFilterMode(M_TEX_FILTER_LINEAR, M_TEX_FILTER_LINEAR);
				render->setTextureUWrapMode(M_WRAP_CLAMP);
				render->setTextureVWrapMode(M_WRAP_CLAMP);
				render->texImage(0, width, height, M_UINT, M_DEPTH, 0);
				render->bindTexture(0);

				colorTexture->setTextureId(m_colorTextureId);
				colorTexture->setWidth(width);
				colorTexture->setHeight(height);
				depthTexture->setTextureId(m_depthTextureId);
				depthTexture->setWidth(width);
				depthTexture->setHeight(height);
			}

			return 0;
		}
	}

	return 0;
}

int disableRenderToTexture()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "disableRenderToTexture", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			OCamera * camera = (OCamera *)object;

			TextureRef * depthTexture = camera->getRenderDepthTexture();
			if(depthTexture)
				depthTexture->clear();

			camera->setRenderColorTexture(NULL);
			camera->setRenderDepthTexture(NULL);

			return 0;
		}
	}

	return 0;
}

int getBehaviorVariable()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getBehaviorVariable", 3))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		unsigned int bId = (unsigned int)script->getInteger(1);
		const char * varName;

		if(script->isNumber(2))
			varName = object->getBehavior(bId)->getVariable(script->getInteger(2)).getName();
		else
			varName = script->getString(2);

		if(varName)
		{
			unsigned bSize = object->getBehaviorsNumber();
			if(bId < bSize)
			{
				Behavior * behavior = object->getBehavior(bId);
				unsigned int v;
				unsigned int vSize = behavior->getVariablesNumber();
				for(v=0; v<vSize; v++)
				{
					NeoVariable variable = behavior->getVariable(v);
					if(strcmp(variable.getName(), varName) == 0)
					{
						switch(variable.getType())
						{
						case M_VARIABLE_BOOL:
							{
								script->pushBoolean( *(bool *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_INT:
						case M_VARIABLE_UINT:
							{
								script->pushInteger( *(int *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_FLOAT:
							{
								script->pushFloat( *(float *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_STRING:
							{
								script->pushString( ((MString *)variable.getPointer())->getSafeString());
								return 1;
							}
						case M_VARIABLE_VEC2:
							{
								script->pushFloatArray( *(MVector2 *)variable.getPointer(), 2);
								return 1;
							}
						case M_VARIABLE_VEC3:
							{
								script->pushFloatArray( *(MVector3 *)variable.getPointer(), 3);
								return 1;
							}
						case M_VARIABLE_VEC4:
							{
								script->pushFloatArray( *(MVector4 *)variable.getPointer(), 4);
								return 1;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

int setBehaviorVariable()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setBehaviorVariable", 4))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		unsigned int bId = (unsigned int)script->getInteger(1);
		const char * varName;

		if(script->isNumber(2))
			varName = object->getBehavior(bId)->getVariable(script->getInteger(2)).getName();
		else
			varName = script->getString(2);

		if(varName)
		{
			unsigned bSize = object->getBehaviorsNumber();
			if(bId < bSize)
			{
				Behavior * behavior = object->getBehavior(bId);
				unsigned int v;
				unsigned int vSize = behavior->getVariablesNumber();
				for(v=0; v<vSize; v++)
				{
					NeoVariable variable = behavior->getVariable(v);
					if(strcmp(variable.getName(), varName) == 0)
					{
						switch(variable.getType())
						{
						case M_VARIABLE_BOOL:
							{
								bool val = script->getBoolean(3);
								*(bool *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_INT:
						case M_VARIABLE_UINT:
							{
								int val = script->getInteger(3);
								*(int *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_FLOAT:
							{
								float val = script->getFloat(3);
								*(float *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_STRING:
							{
								const char * str = script->getString(3);
								if(str)
									((MString *)variable.getPointer())->set(str);
								return 0;
							}
						case M_VARIABLE_VEC2:
							{
								MVector2 vec;
								if(getVector2(script, 4, &vec))
									*(MVector2 *)variable.getPointer() = vec;
								return 0;
							}
						case M_VARIABLE_VEC3:
							{
								MVector3 vec;
								if(getVector3(script, 4, &vec))
									*(MVector3 *)variable.getPointer() = vec;
								return 0;
							}
						case M_VARIABLE_VEC4:
							{
								MVector4 vec;
								if(getVector4(script, 4, &vec))
									*(MVector4 *)variable.getPointer() = vec;
								return 0;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

int getBehaviorsNumber()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getBehaviorsNumber", 1))
		return 0;

	Object3d* object = (Object3d*)(script->getPointer(0));
	if(object)
	{
		script->pushFloat( object->getBehaviorsNumber());
		return 1;
	}

	script->pushFloat( -1);
	return 1;
}

int getBehaviorName()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getBehaviorName", 2))
		return 0;

	Object3d* object = (Object3d*)(script->getPointer(0));
	int behavior = script->getFloat(1);
	if(object && behavior >= 1 && behavior <= object->getBehaviorsNumber())
	{
		script->pushString( object->getBehavior(behavior-1)->getName());
		return 1;
	}
	return 0;
}

int getBehaviorVariablesNumber()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getBehaviorVariablesNumber", 2))
		return 0;

	Object3d* object = (Object3d*)(script->getPointer(0));
	int behavior = script->getFloat(1);
	if(object && behavior >= 1 && behavior <= object->getBehaviorsNumber())
	{
		script->pushFloat( object->getBehavior(behavior-1)->getVariablesNumber());
		return 1;
	}

	script->pushFloat( -1);
	return 0;
}

int getBehaviorVariableType()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getBehaviorVariablesNumber", 3))
		return 0;

	Object3d* object = (Object3d*)(script->getPointer(0));
	int behavior = script->getFloat(1);
	int variable = script->getFloat(2);

	if(object && behavior >= 1 && behavior <= object->getBehaviorsNumber())
	{
		switch(object->getBehavior(behavior-1)->getVariable(variable-1).getType())
		{
		case M_VARIABLE_BOOL:
			script->pushString( "bool");
			break;
		case M_VARIABLE_VEC2:
			script->pushString( "vec2");
			break;
		case M_VARIABLE_VEC3:
			script->pushString( "vec3");
			break;
		case M_VARIABLE_VEC4:
			script->pushString( "vec4");
			break;
		case M_VARIABLE_FLOAT:
			script->pushString( "float");
			break;
		case M_VARIABLE_INT:
			script->pushString( "int");
			break;
		case M_VARIABLE_STRING:
			script->pushString( "string");
			break;
		case M_VARIABLE_UINT:
			script->pushString( "uint");
			break;
		}

		return 1;
	}

	return 0;
}

// addBehavior(object, behaviorName)
int addBehavior()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "addBehavior", 2))
		return 0;

	Object3d* object = (Object3d*)(script->getPointer(0));
	const char* name = script->getString(1);
	if(object)
	{
		BehaviorManager * bManager = NeoEngine::getInstance()->getBehaviorManager();

		if(!name)
		{
			MLOG_ERROR("Unable to load behavior!");
			return 0;
		}

		BehaviorCreator * bCreator = bManager->getBehaviorByName(name);

		if(!bCreator)
		{
			MLOG_ERROR("Unable to load behavior \"" << name << "\"");
			return 0;
		}

		Behavior* behavior = bCreator->getNewBehavior(object);
		object->addBehavior(behavior);
		return 1;
	}

	MLOG_ERROR("Unable to load behavior!");
	return 0;
}

int centerCursor()
{
	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	MInputContext * input = engine->getInputContext();

	unsigned int width = 0;
	unsigned int height = 0;
	system->getScreenSize(&width, &height);
	int x = width/2;
	int y = height/2;

	system->setCursorPosition(x, y);
	input->setAxis("MOUSE_X", (float)(x / (float)width));
	input->setAxis("MOUSE_Y", (float)(y / (float)height));

	return 0;
}

int hideCursor()
{
	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	system->hideCursor();

	return 0;
}

int showCursor()
{
	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	system->showCursor();

	return 0;
}

int loadTextFont()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "loadTextFont", 1))
		return 0;

	const char* path = script->getString(0);

	Level* level = NeoEngine::getInstance()->getLevel();
	Scene* scene = level->getCurrentScene();

	char string[256];
	getGlobalFilename(string, NeoWindow::getInstance()->getWorkingDirectory(), path);

	if(!isFileExist(string))
	{
		MLOG_ERROR("Could not load font file '" << path << "': File does not exist!");
		return 0;
	}

	FontRef* ref = level->loadFont(string);
	OText* text = scene->addNewText(ref);

	strcpy(string, "Text0");
	getNewObjectName("Text", string);

	text->setName(string);
	text->setText("Text");

	script->pushInteger( (long int) text);

	return 1;
}

int getFontFilename()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getFontFilename", 1))
		return 0;

	int nbArguments = script->getArgsNumber();

	Object3d* object = (Object3d*) script->getPointer(nbArguments-1);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			OText* text = (OText*) object;
			script->pushString( text->getFontRef()->getFilename());
			return 1;
		}
	}

	return 0;
}

int getTextAlignment()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getTextAlignment", 1))
		return 0;

	Object3d* object = (Object3d*) script->getPointer(0);

	if(object)
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			switch(((OText*) object)->getAlign())
			{
			case TEXT_ALIGN_CENTER:
					script->pushString( "Center");
				break;
			case TEXT_ALIGN_LEFT:
					script->pushString( "Left");
				break;
			case TEXT_ALIGN_RIGHT:
					script->pushString( "Right");
				break;
			}
			return 1;
		}
	}

	return 0;
}

int setTextAlignment()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setTextAlignment", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);
	const char* alignment = script->getString(1);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			if(!strcmp(alignment, "Center"))
				static_cast<OText*>(object)->setAlign(TEXT_ALIGN_CENTER);
			else if(!strcmp(alignment, "Left"))
				static_cast<OText*>(object)->setAlign(TEXT_ALIGN_LEFT);
			else if(!strcmp(alignment, "Right"))
				static_cast<OText*>(object)->setAlign(TEXT_ALIGN_RIGHT);
			else
				MLOG_WARNING("Unknown alignment: " << alignment);

			return 1;
		}
	}

	return 0;
}

int getTextFontSize()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getTextFontSize", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			script->pushFloat( ((OText*) object)->getSize());
			return 1;
		}
	}

	return 0;
}

int setTextFontSize()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setTextFontSize", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			((OText *)object)->setSize(script->getFloat(1));
		}
	}

	return 0;
}

int getText()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getText", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			const char * text = ((OText *)object)->getText();
			if(text){
				script->pushString( text);
				return 1;
			}
		}
	}

	return 0;
}

int setText()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setText", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		const char * text = script->getString(1);
		if((text) && (object->getType() == M_OBJECT3D_TEXT))
		{
			((OText *)object)->setText(text);
		}
	}

	return 0;
}

int getTextColor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "getTextColor", 1))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			MVector4 color = ((OText *)object)->getColor();
			script->pushFloatArray(color, 4);
			return 1;
		}
	}

	return 0;
}

int setTextColor()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setTextColor", 2))
		return 0;

	Object3d * object;
	Object3d* id = (Object3d*) script->getPointer(0);

	if((object = (Object3d*) id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			MVector4 color;
			script->getFloatArray(1, color, 4);
			((OText *)object)->setColor(color);
		}
	}

	return 0;
}

int getWindowScale()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	unsigned int width, height;
	system->getScreenSize(&width, &height);

	float scale[2] = {width, height};
	script->pushFloatArray(scale, 2);

	return 1;
}

int getSystemTick()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoEngine * engine = NeoEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	script->pushInteger( (long int)(system->getSystemTick() - g_startTick));
	return 1;
}

int doFile()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "doFile", 1))
		return 0;

	const char * filename = script->getString(0);

	// save current directory
	char current[256];
	strcpy(current, g_currentDirectory);

	// make global filename
	char globalFilename[256];
	getGlobalFilename(globalFilename, g_currentDirectory, filename);

	// read text
	char * text = readTextFile(globalFilename);
	if(!text)
	{
		MLOG_ERROR("Could not load script file: " << filename);
		return 0;
	}

	// update current directory
	getRepertory(g_currentDirectory, globalFilename);

	// do string
	if(!script->runString(text))
	{
		MLOG_ERROR("Could not execute " << filename);
	}

	// set back to current
	strcpy(g_currentDirectory, current);

	SAFE_FREE(text);
	return 0;
}

int setPhysicsQuality()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(! isFunctionOk(script, "setPhysicsQuality", 1))
		return 0;

	int quality = script->getInteger(0);
	NeoEngine::getInstance()->getPhysicsContext()->setSimulationQuality(quality);

	return 1;
}

int loadCameraSkybox()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "loadCameraSkybox", 2))
		return 0;

	GET_OBJECT_SUBCLASS_BEGIN(OCamera, camera, M_OBJECT3D_CAMERA)
			camera->loadSkybox(script->getString(1));
			return 1;
	GET_OBJECT_SUBCLASS_END()

	return 0;
}

// enablePostEffects(vertShadSrc, fragShadSrc)
int enablePostEffects()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	int num_args = script->getArgsNumber();
	NeoGame* game = NeoEngine::getInstance()->getGame();

	if(num_args == 0)
	{
		game->enablePostEffects();
	}

	if(num_args == 2)
	{
		game->enablePostEffects();
		game->getPostProcessor()->loadShader(script->getString(0), script->getString(1));
	}

	return 0;
}

// loadPostEffectsShader(vertShadFile, fragShadFile)
int loadPostEffectsShader()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "loadPostEffectsShader", 2))
		return 0;

	char* vertShad = readTextFile(script->getString(0));
	char* fragShad = readTextFile(script->getString(1));

	NeoGame* game = NeoEngine::getInstance()->getGame();
	game->getPostProcessor()->loadShader(vertShad, fragShad);

	SAFE_FREE(vertShad);
	SAFE_FREE(fragShad);
	return 1;
}

int disablePostEffects()
{
	NeoGame* game = NeoEngine::getInstance()->getGame();
	game->disablePostEffects();
	return 1;
}

int setPostEffectsResolution()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setPostEffectsResolution", 1))
		return 0;

	float mp = script->getFloat(0);
	NeoGame* game = NeoEngine::getInstance()->getGame();
	MPostProcessor* pp = game->getPostProcessor();
	pp->setResolutionMultiplier(mp);
	pp->eraseTextures();
	pp->updateResolution();

	return 1;
}

int getPostEffectsResolution()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	NeoGame* game = NeoEngine::getInstance()->getGame();
	script->pushFloat( game->getPostProcessor()->getResolutionMultiplier());
	return 1;
}

int setPostEffectsUniformFloat()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setPostEffectsUniformFloat", 2))
		return 0;

	NeoGame* game = NeoEngine::getInstance()->getGame();
	game->getPostProcessor()->setFloatUniformValue(script->getString(0), script->getFloat(1));
	return 1;
}

int addPostEffectsUniformFloat()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setPostEffectsUniformFloat", 2))
		return 0;

	NeoGame* game = NeoEngine::getInstance()->getGame();
	game->getPostProcessor()->addFloatUniform(script->getString(0));
	game->getPostProcessor()->setFloatUniformValue(script->getString(0), script->getFloat(1));
	return 1;
}

int setPostEffectsUniformInt()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setPostEffectsUniformInt", 2))
		return 0;

	NeoGame* game = NeoEngine::getInstance()->getGame();
	game->getPostProcessor()->setIntUniformValue(script->getString(0), script->getFloat(1));
	return 1;
}

int addPostEffectsUniformInt()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "setPostEffectsUniformInt", 2))
		return 0;

	NeoGame* game = NeoEngine::getInstance()->getGame();
	game->getPostProcessor()->addIntUniform(script->getString(0));
	game->getPostProcessor()->setIntUniformValue(script->getString(0), script->getFloat(1));
	return 1;
}

int resizeWindow()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!isFunctionOk(script, "resizeWindow", 2))
		return 0;

	NeoWindow::getInstance()->resize(script->getInteger(0), script->getInteger(1));
	return 1;
}

int getWorkingDirectory()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	script->pushString(NeoWindow::getInstance()->getWorkingDirectory());
	return 1;
}

int loadTextFile()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!isFunctionOk(script, "loadTextFile", 1))
        return 0;

    const char* content = readTextFile(script->getString(0));
    script->pushString(content);

    return 1;
}

void Neo::bindLuaApi(MScriptContext* context)
{
	context->addFunction( "vec3", vec3);
	context->addFunction( "length", length);
	context->addFunction( "normalize", normalize);
	context->addFunction( "dot", dot);
	context->addFunction( "cross", cross);

	// object/scene init
	context->addFunction( "getScene",	 getScene);
	context->addFunction( "getObject",	 getObject);
	context->addFunction( "objectExists", objectExists);
	context->addFunction( "getClone",	 getClone);
	context->addFunction( "getParent",	 getParent);
	context->addFunction( "getChilds",	 getChilds);
	context->addFunction( "getCurrentCamera",    getCurrentCamera);
	context->addFunction( "loadCameraSkybox", loadCameraSkybox);
	context->addFunction( "deleteObject", deleteObject);
	context->addFunction( "resizeWindow", resizeWindow);

	// object
	context->addFunction( "rotate",					rotate);
	context->addFunction( "translate",				translate);
	context->addFunction( "getPosition",			getPosition);
	context->addFunction( "getRotation",			getRotation);
	context->addFunction( "getScale",				getScale);
	context->addFunction( "setPosition",			setPosition);
	context->addFunction( "setRotation",			setRotation);
	context->addFunction( "setScale",				setScale);
	context->addFunction( "isVisible",				isVisible);
	context->addFunction( "setInvisible",           setInvisible);
	context->addFunction( "activate",				activate);
	context->addFunction( "deactivate",				deactivate);
	context->addFunction( "isActive",				isActive);
	context->addFunction( "getName",				getName);
	context->addFunction( "setParent",				setParent);

	context->addFunction( "enableShadow",			enableShadow);
	context->addFunction( "isCastingShadow",		isCastingShadow);
	context->addFunction( "getMeshFilename",        getMeshFilename);
	context->addFunction( "loadMesh",               loadMesh);
	context->addFunction( "getObjectType",          getObjectType);
	context->addFunction( "getBoundingMax",         getBoundingMax);
	context->addFunction( "getBoundingMin",         getBoundingMin);
	context->addFunction( "createGroup",            createGroup);

	context->addFunction( "getTransformedPosition", getTransformedPosition);
	context->addFunction( "getTransformedRotation", getTransformedRotation);
	context->addFunction( "getTransformedScale",	getTransformedScale);
	context->addFunction( "getInverseRotatedVector",getInverseRotatedVector);
	context->addFunction( "getRotatedVector",		getRotatedVector);
	context->addFunction( "getInverseVector",		getInverseVector);
	context->addFunction( "getTransformedVector",	getTransformedVector);
	context->addFunction( "updateMatrix",			updateMatrix);
	context->addFunction( "getMatrix",				getMatrix);

	// behavior
	context->addFunction( "getBehaviorVariable",	getBehaviorVariable);
	context->addFunction( "setBehaviorVariable",	setBehaviorVariable);
	context->addFunction( "getBehaviorsNumber",     getBehaviorsNumber);
	context->addFunction( "getBehaviorName",        getBehaviorName);
	context->addFunction( "getBehaviorVariablesNumber", getBehaviorVariablesNumber);
	context->addFunction( "getBehaviorVariableType",    getBehaviorVariableType);
	context->addFunction( "addBehavior",            addBehavior);

	// animation
	context->addFunction( "getCurrentAnimation",	getCurrentAnimation);
	context->addFunction( "changeAnimation",		changeAnimation);
	context->addFunction( "isAnimationOver",		isAnimationOver);
	context->addFunction( "getAnimationSpeed",		getAnimationSpeed);
	context->addFunction( "setAnimationSpeed",		setAnimationSpeed);
	context->addFunction( "getCurrentFrame",		getCurrentFrame);
	context->addFunction( "setCurrentFrame",		setCurrentFrame);

	// physics
	context->addFunction( "enablePhysics",		enablePhysics);
	context->addFunction( "setGravity",			setGravity);
	context->addFunction( "getGravity",			getGravity);
	context->addFunction( "addCentralForce",	addCentralForce);
	context->addFunction( "addTorque",			addTorque);
	context->addFunction( "getLinearDamping",	getLinearDamping);
	context->addFunction( "setLinearDamping",	setLinearDamping);
	context->addFunction( "getAngularDamping",	getAngularDamping);
	context->addFunction( "setAngularDamping",	setAngularDamping);
	context->addFunction( "getMass",			getMass);
	context->addFunction( "setMass",			setMass);
	context->addFunction( "getFriction",		getFriction);
	context->addFunction( "setFriction",		setFriction);
	context->addFunction( "getRestitution",		getRestitution);
	context->addFunction( "setRestitution",		setRestitution);
	context->addFunction( "getAngularFactor",	getAngularFactor);
	context->addFunction( "setAngularFactor",	setAngularFactor);
	context->addFunction( "getLinearFactor",	getLinearFactor);
	context->addFunction( "setLinearFactor",	setLinearFactor);
	context->addFunction( "getCentralForce",	getCentralForce);
	context->addFunction( "getTorque",			getTorque);

	context->addFunction( "isCollisionTest",	isCollisionTest);
	context->addFunction( "isCollisionBetween", isCollisionBetween);
	context->addFunction( "clearForces",		clearForces);
	context->addFunction( "getNumCollisions",	getNumCollisions);
	context->addFunction( "rayHit",				rayHit);

	context->addFunction( "setPhysicsQuality",  setPhysicsQuality);
	context->addFunction( "setConstraintParent", setConstraintParent);
	context->addFunction( "getConstraintParent", getConstraintParent);
	context->addFunction( "enableParentCollision", enableParentCollision);
	context->addFunction( "isGhost", isGhost);
	context->addFunction( "enableGhost", enableGhost);

	// input
	context->addFunction( "isKeyPressed", isKeyPressed);
	context->addFunction( "onKeyDown",	  onKeyDown);
	context->addFunction( "onKeyUp",	  onKeyUp);
	context->addFunction( "getAxis",	  getAxis);
	context->addFunction( "getProperty",  getProperty);

	// multitouch
	context->addFunction( "getTouchPosition", getTouchPosition);
	context->addFunction( "getLastTouchPosition", getLastTouchPosition);
	context->addFunction( "getTouchPhase", getTouchPhase);

	// sound
	context->addFunction( "loadSound",          loadSound);
	context->addFunction( "getSoundFilename",   getSoundFilename);
	context->addFunction( "playSound",          playSound);
	context->addFunction( "pauseSound",         pauseSound);
	context->addFunction( "stopSound",          stopSound);
	context->addFunction( "getSoundGain",       getSoundGain);
	context->addFunction( "setSoundGain",       setSoundGain);
	context->addFunction( "setSoundPitch",      setSoundPitch);
	context->addFunction( "getSoundPitch",      getSoundPitch);

	context->addFunction( "getSoundRolloff",    getSoundRolloff);
	context->addFunction( "getSoundRadius",     getSoundRadius);
	context->addFunction( "isSoundRelative",    isSoundRelative);
	context->addFunction( "isSoundLooping",     isSoundLooping);

	context->addFunction( "setSoundLooping",    setSoundLooping);
	context->addFunction( "setSoundRelative",   setSoundRelative);
	context->addFunction( "setSoundRadius",     setSoundRadius);
	context->addFunction( "setSoundRolloff",    setSoundRolloff);

	// scene/level
	context->addFunction( "changeScene",			changeScene);
	context->addFunction( "getCurrentSceneId",		getCurrentSceneId);
	context->addFunction( "getScenesNumber",		getScenesNumber);
	context->addFunction( "doesLevelExist",			doesLevelExist);
	context->addFunction( "loadLevel",				loadLevel);
	context->addFunction( "enablePostEffects",      enablePostEffects);
	context->addFunction( "disablePostEffects",     disablePostEffects);
	context->addFunction( "loadPostEffectsShader",  loadPostEffectsShader);
	context->addFunction( "setPostEffectsUniformFloat", setPostEffectsUniformFloat);
	context->addFunction( "addPostEffectsUniformFloat", addPostEffectsUniformFloat);
	context->addFunction( "setPostEffectsUniformInt", setPostEffectsUniformInt);
	context->addFunction( "addPostEffectsUniformInt", addPostEffectsUniformInt);
	context->addFunction( "setPostEffectsResolution", setPostEffectsResolution);
	context->addFunction( "getPostEffectsResolution", getPostEffectsResolution);

	// light
	context->addFunction( "createLight",       createLight);
	context->addFunction( "getLightColor",	   getLightColor);
	context->addFunction( "getLightRadius",	   getLightRadius);
	context->addFunction( "getLightIntensity", getLightIntensity);
	context->addFunction( "setLightColor",	   setLightColor);
	context->addFunction( "setLightRadius",	   setLightRadius);
	context->addFunction( "setLightIntensity", setLightIntensity);
	context->addFunction( "setLightShadowQuality",	setlightShadowQuality);
	context->addFunction( "setLightShadowBias",		setlightShadowBias);
	context->addFunction( "setLightShadowBlur",		setlightShadowBlur);
	context->addFunction( "setLightSpotAngle",		setlightSpotAngle);
	context->addFunction( "setLightSpotExponent",	setlightSpotExponent);
	context->addFunction( "getLightShadowQuality",	getlightShadowQuality);
	context->addFunction( "getLightShadowBias",		getlightShadowBias);
	context->addFunction( "getLightShadowBlur",		getlightShadowBlur);
	context->addFunction( "getLightSpotAngle",		getlightSpotAngle);
	context->addFunction( "getLightSpotExponent",	getlightSpotExponent);

	// camera
	context->addFunction( "createCamera",           createCamera);
	context->addFunction( "changeCurrentCamera",    changeCurrentCamera);
	context->addFunction( "getCameraClearColor",    getCameraClearColor);
	context->addFunction( "getCameraFov",		    getCameraFov);
	context->addFunction( "getCameraNear",		    getCameraNear);
	context->addFunction( "getCameraFar",		    getCameraFar);
	context->addFunction( "getCameraFogDistance",   getCameraFogDistance);
	context->addFunction( "isCameraOrtho",		    isCameraOrtho);
	context->addFunction( "isCameraFogEnabled",	    isCameraFogEnabled);
	context->addFunction( "setCameraClearColor",    setCameraClearColor);
	context->addFunction( "setCameraFov",		    setCameraFov);
	context->addFunction( "setCameraNear",	        setCameraNear);
	context->addFunction( "setCameraFar",	        setCameraFar);
	context->addFunction( "setCameraFogDistance",   setCameraFogDistance);
	context->addFunction( "enableCameraOrtho",      enableCameraOrtho);
	context->addFunction( "enableCameraFog",	    enableCameraFog);
	context->addFunction( "enableCameraLayer",      enableCameraLayer);
	context->addFunction( "disableCameraLayer",	    disableCameraLayer);
	context->addFunction( "enableRenderToTexture",  enableRenderToTexture);
	context->addFunction( "disableRenderToTexture", disableRenderToTexture);
	context->addFunction( "getProjectedPoint",		getProjectedPoint);
	context->addFunction( "getUnProjectedPoint",	getUnProjectedPoint);

	// text
	context->addFunction( "loadTextFont", loadTextFont);
	context->addFunction( "getFontFilename", getFontFilename);
	context->addFunction( "getTextFontSize", getTextFontSize);
	context->addFunction( "setTextFontSize", setTextFontSize);
	context->addFunction( "getText", getText);
	context->addFunction( "setText", setText);
	context->addFunction( "getTextColor", getTextColor);
	context->addFunction( "setTextColor", setTextColor);
	context->addFunction( "setTextAlignment", setTextAlignment);
	context->addFunction( "getTextAlignment", getTextAlignment);

	// do file
	context->addFunction( "dofile", doFile);

	// global
	context->addFunction( "centerCursor",	centerCursor);
	context->addFunction( "hideCursor",		hideCursor);
	context->addFunction( "showCursor",		showCursor);
	context->addFunction( "getWindowScale", getWindowScale);
	context->addFunction( "getSystemTick",	getSystemTick);
	context->addFunction( "quit",			quit);
	context->addFunction( "getWorkingDirectory", getWorkingDirectory);
    context->addFunction("loadTextFile", loadTextFile);
}
