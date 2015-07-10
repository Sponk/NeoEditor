//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#include "../Includes/NeoEngine.h"

using namespace Neo;

Scene::Scene():
m_dataMode(M_DATA_STATIC),
m_currentFrame(0),
m_currentCamera(0),
m_gravity(0, 0, -0.981f),
m_additionalData(NULL)
{}

Scene::~Scene()
{
	stopAllSounds();

	// delete objects
	unsigned int oSize = m_objects.size();
	for(unsigned int i=0; i<oSize; i++)
		SAFE_DELETE(m_objects[i]);

	m_objects.clear();

	m_entities.clear();
	m_cameras.clear();
	m_lights.clear();
	m_sounds.clear();

	SAFE_DELETE(m_additionalData);
}

Object3d * Scene::addNewGroup(void)
{
    Object3d * object = new Object3d();

	object->setId(++m_ids);
	m_handles[object->getId()] = object;

    m_objects.push_back(object);
    return object;
}

Object3d * Scene::addNewGroup(const Object3d & object)
{
    Object3d * group = new Object3d();
    m_objects.push_back(group);

	group->setId(++m_ids);
	m_handles[group->getId()] = group;

    group->setPosition(object.getTransformedPosition());
    group->setRotation(object.getRotation());
    group->setScale(object.getTransformedScale());
    group->updateMatrix();

    return group;
}

OCamera * Scene::addNewCamera(void)
{
	OCamera * newCamera = new OCamera();
	newCamera->setId(++m_ids);
	m_handles[newCamera->getId()] = newCamera;

	m_cameras.push_back(newCamera);
	m_objects.push_back(newCamera);
	return newCamera;
}

OCamera * Scene::addNewCamera(const OCamera & camera)
{
	OCamera * newCamera = new OCamera(camera);
	newCamera->setId(++m_ids);

	m_handles[newCamera->getId()] = newCamera;

	m_cameras.push_back(newCamera);
	m_objects.push_back(newCamera);
	return newCamera;
}

OLight * Scene::addNewLight(void)
{
	OLight * newLight = new OLight();

	newLight->setId(++m_ids);
	m_handles[newLight->getId()] = newLight;

	m_lights.push_back(newLight);
	m_objects.push_back(newLight);
	return newLight;
}

OLight * Scene::addNewLight(const OLight & light)
{
	OLight * newLight = new OLight(light);

	newLight->setId(++m_ids);
	m_handles[newLight->getId()] = newLight;

	m_lights.push_back(newLight);
	m_objects.push_back(newLight);
	return newLight;
}

OEntity * Scene::addNewEntity(MeshRef * meshRef)
{
	OEntity * newEntity = new OEntity(meshRef);

	newEntity->setId(++m_ids);
	m_handles[newEntity->getId()] = newEntity;

	m_entities.push_back(newEntity);
	m_objects.push_back(newEntity);
	return newEntity;
}

OEntity * Scene::addNewEntity(const OEntity & entity)
{
	OEntity * newEntity = new OEntity(entity);

	newEntity->setId(++m_ids);
	m_handles[newEntity->getId()] = newEntity;

	m_entities.push_back(newEntity);
	m_objects.push_back(newEntity);
	return newEntity;
}

OSound * Scene::addNewSound(SoundRef * soundRef)
{
	OSound * newSoundSource = new OSound(soundRef);

	newSoundSource->setId(++m_ids);
	m_handles[newSoundSource->getId()] = newSoundSource;

	m_sounds.push_back(newSoundSource);
	m_objects.push_back(newSoundSource);
	return newSoundSource;
}

OSound * Scene::addNewSound(const OSound & sound)
{
	OSound * newSoundSource = new OSound(sound);

	newSoundSource->setId(++m_ids);
	m_handles[newSoundSource->getId()] = newSoundSource;

	m_sounds.push_back(newSoundSource);
	m_objects.push_back(newSoundSource);
	return newSoundSource;
}

OText * Scene::addNewText(FontRef * fontRef)
{
	OText * newText = new OText(fontRef);

	newText->setId(++m_ids);
	m_handles[newText->getId()] = newText;

	m_texts.push_back(newText);
	m_objects.push_back(newText);
	return newText;
}

OText * Scene::addNewText(const OText & text)
{
	OText * newText = new OText(text);

	newText->setId(++m_ids);
	m_handles[newText->getId()] = newText;

	m_texts.push_back(newText);
	m_objects.push_back(newText);
	return newText;
}

OCamera * Scene::getCurrentCamera(void)
{
	unsigned int cSize = getCamerasNumber();
	if(cSize > 0)
	{
		if(m_currentCamera < cSize)
			return getCameraByIndex(m_currentCamera);
		else
			return getCameraByIndex(0);
	}
	
	return NULL;
}

void Scene::setName(const char * name)
{
	m_name.set(name);
}

void Scene::setScriptFilename(const char * scriptFilename)
{
	m_scriptFilename.set(scriptFilename);
}

bool createShape(OEntity * entity, PhysicsProperties * phyProps, unsigned int * shapeId)
{
	PhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();

	// get bounding box
	Box3d * box = entity->getBoundingBox();

	Vector3 scale = entity->getTransformedScale();

	// swith shapes
	switch(phyProps->getCollisionShape())
	{
	default:
	case COLLISION_SHAPE_BOX:
		physics->createBoxShape(shapeId, (box->max - box->min)*scale*0.5f);
		break;

	case COLLISION_SHAPE_SPHERE:
		{
			Vector3 vec = (box->max - box->min)*scale*0.5f;
			float radius = vec.x;
			radius = MAX(radius, vec.y);
			radius = MAX(radius, vec.z);
			physics->createSphereShape(shapeId, radius);
		}
		break;

	case COLLISION_SHAPE_CONE:
		{
			Vector3 vec = (box->max - box->min)*scale;
			float height = vec.y;
			float radius = vec.x*0.5f;
			radius = MAX(radius, vec.z*0.5f);
			physics->createConeShape(shapeId, radius, height);
		}
		break;

	case COLLISION_SHAPE_CAPSULE:
		{
			Vector3 vec = (box->max - box->min)*scale;
			float height = vec.y;
			float radius = vec.x*0.5f;
			radius = MAX(radius, vec.z*0.5f);
			physics->createCylinderShape(shapeId, radius, height);
		}
		break;

	case COLLISION_SHAPE_CYLINDER:
		{
			Vector3 vec = (box->max - box->min)*scale;
			float height = vec.y;
			float radius = vec.x*0.5f;
			radius = MAX(radius, vec.z*0.5f);
			physics->createCylinderShape(shapeId, radius, height);
		}
		break;

	case COLLISION_SHAPE_CONVEX_HULL:
		{
			Mesh * mesh = entity->getMesh();
			if(mesh)
			{
				SubMesh * subMeshs = mesh->getSubMeshs();
				unsigned int subMeshsNumber = mesh->getSubMeshsNumber();
				if(subMeshsNumber == 0)
					return false;

				if(subMeshsNumber == 1)
				{
					SubMesh * subMesh = &subMeshs[0];
					if(subMesh->getVerticesSize() > 0)
						physics->createConvexHullShape(shapeId, subMesh->getVertices(), subMesh->getVerticesSize(), entity->getScale());
				}
				else
				{
					physics->createMultiShape(shapeId);
					unsigned int s;
					for(s=0; s<subMeshsNumber; s++)
					{
						unsigned int subShapeId;
						SubMesh * subMesh = &subMeshs[s];
						if(subMesh->getVerticesSize() > 0)
						{
							physics->createConvexHullShape(&subShapeId, subMesh->getVertices(), subMesh->getVerticesSize(), entity->getScale());
							physics->addChildShape(*shapeId, subShapeId, Vector3(), Quaternion());
						}
					}
				}
			}
			else{
				return false;
			}
		}
		break;

	case COLLISION_SHAPE_TRIANGLE_MESH:
		{
			Mesh * mesh = entity->getMesh();
			if(mesh)
			{
				SubMesh * subMeshs = mesh->getSubMeshs();
				unsigned int subMeshsNumber = mesh->getSubMeshsNumber();
				if(subMeshsNumber == 0)
					return false;

				if(subMeshsNumber == 1)
				{
					SubMesh * subMesh = &subMeshs[0];
					if(subMesh->getVerticesSize() >= 3)
						physics->createTriangleMeshShape(shapeId,
							subMesh->getVertices(), subMesh->getVerticesSize(),
							subMesh->getIndices(), subMesh->getIndicesSize(), subMesh->getIndicesType(),
							entity->getScale()
						);
				}
				else
				{
					physics->createMultiShape(shapeId);
					unsigned int s;
					for(s=0; s<subMeshsNumber; s++)
					{
						unsigned int subShapeId;
						SubMesh * subMesh = &subMeshs[s];
						if(subMesh->getVerticesSize() >= 3)
						{
							physics->createTriangleMeshShape(&subShapeId,
								subMesh->getVertices(), subMesh->getVerticesSize(),
								subMesh->getIndices(), subMesh->getIndicesSize(), subMesh->getIndicesType(),
								entity->getScale()
							);
							physics->addChildShape(*shapeId, subShapeId, Vector3(), Quaternion());
						}
					}
				}
			}
			else{
				return false;
			}
		}
		break;
	}

	return true;
}

void Scene::prepareCollisionShape(OEntity * entity)
{
	PhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
	PhysicsProperties * phyProps = entity->getPhysicsProperties();

	if(! phyProps)
		return;

	unsigned int shapeId = 0;
	if(createShape(entity, phyProps, &shapeId))
	{
		// has physics child
		bool hasPhysicsChild = false;
		unsigned int o;
		unsigned int oSize = entity->getChildrenNumber();
		for(o=0; o<oSize; o++)
		{
			Object3d * childObject = entity->getChild(o);
			if(childObject->getType() == M_OBJECT3D_ENTITY)
			{
				OEntity * childEntity = (OEntity*)childObject;
				PhysicsProperties * childPhyProps = childEntity->getPhysicsProperties();
				if(childPhyProps)
				{
					if(! childPhyProps->isGhost())
					{
						hasPhysicsChild = true;
						break;
					}
				}
			}
		}

		// create multi shape
		if(hasPhysicsChild)
		{
			unsigned int subShapeId = shapeId;
			physics->createMultiShape(&shapeId);
			physics->addChildShape(shapeId, subShapeId, Vector3(), Quaternion());
		}

		phyProps->setShapeId(shapeId);
	}
}

void Scene::prepareCollisionObject(OEntity * entity)
{
	PhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
	PhysicsProperties * phyProps = entity->getPhysicsProperties();
	if(! phyProps)
		return;

	unsigned int shapeId = phyProps->getShapeId();
	if(shapeId == 0)
		return;

	// has physics parent
	PhysicsProperties * parentPhyProps = NULL;
	OEntity * parentEntity = NULL;
	if(! phyProps->isGhost())
	{
		Object3d * parentObject = entity->getParent();
		if(parentObject)
		{
			if(parentObject->getType() == M_OBJECT3D_ENTITY)
			{
				parentEntity = (OEntity*)parentObject;
				parentPhyProps = parentEntity->getPhysicsProperties();
			}
		}
	}

	if(parentPhyProps) // add shape to parent multi-shape
	{
		Vector3 position = entity->getPosition() * parentEntity->getTransformedScale();
		Quaternion rotation = entity->getRotation();

		phyProps->setShapeId(shapeId);
		physics->addChildShape(parentPhyProps->getShapeId(), shapeId, position, rotation);
	}
	else // create collision object
	{
		unsigned int collisionObjectId;

		if(phyProps->isGhost())
		{
			Vector3 euler = entity->getTransformedRotation();

			physics->createGhost(
				&collisionObjectId, shapeId,
				entity->getTransformedPosition(),
				Quaternion(euler.x, euler.y, euler.z)
			);

			phyProps->setShapeId(shapeId);
			phyProps->setCollisionObjectId(collisionObjectId);
		}
		else
		{
			physics->createRigidBody(
				&collisionObjectId, shapeId,
				entity->getPosition(), entity->getRotation(),
				phyProps->getMass()
			);

			phyProps->setShapeId(shapeId);
			phyProps->setCollisionObjectId(collisionObjectId);

			physics->setObjectRestitution(collisionObjectId, phyProps->getRestitution());
			physics->setObjectDamping(collisionObjectId, phyProps->getLinearDamping(), phyProps->getAngularDamping());
			physics->setObjectAngularFactor(collisionObjectId, phyProps->getAngularFactor());
			physics->setObjectLinearFactor(collisionObjectId, *phyProps->getLinearFactor());
			physics->setObjectFriction(collisionObjectId, phyProps->getFriction());
		}

		// deactivate
		if(! entity->isActive())
			physics->deactivateObject(collisionObjectId);

		// set user pointer (entity)
		physics->setObjectUserPointer(collisionObjectId, entity);
	}
}

void Scene::prepareConstraints(OEntity * entity)
{
	PhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
	PhysicsProperties * phyProps = entity->getPhysicsProperties();

	if(! phyProps)
		return;

	unsigned int objectId = phyProps->getCollisionObjectId();
	PhysicsConstraint * constraint = phyProps->getConstraint();

	if((! constraint) || (objectId == 0) || phyProps->isGhost())
		return;

	if(! entity->getParent())
	{
		OEntity * constraintParent = getEntityByName(constraint->parentName.getSafeString());
		if(constraintParent)
		{
			PhysicsProperties * parentPhyProps = constraintParent->getPhysicsProperties();
			if(parentPhyProps)
			{
				unsigned int parentObjectId = parentPhyProps->getCollisionObjectId();
				if(parentObjectId != 0)
				{
					physics->createConstraint(
											  &constraint->constraintId,
											  parentObjectId,
											  objectId,
											  constraint->pivot,
											  constraint->disableParentCollision
											  );

					constraint->parentObjectId = parentObjectId;
					physics->setLinearLimit(constraint->constraintId, constraint->lowerLinearLimit, constraint->upperLinearLimit);
					physics->setAngularLimit(constraint->constraintId, constraint->lowerAngularLimit, constraint->upperAngularLimit);
				}
			}
		}
	}
}

void Scene::preparePhysics(void)
{
	PhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return;

	physics->init(Vector3(-10000), Vector3(10000));
	physics->setWorldGravity(m_gravity);

	// create shapes
	unsigned int i;
	unsigned int size = getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		OEntity * entity = getEntityByIndex(i);
		prepareCollisionShape(entity);
	}

	// create objects
	for(i=0; i<size; i++)
	{
		OEntity * entity = getEntityByIndex(i);
		prepareCollisionObject(entity);
	}

	// create constraints
	for(i=0; i<size; i++)
	{
		OEntity * entity = getEntityByIndex(i);
		prepareConstraints(entity);
	}
}

void Scene::deleteObject(Object3d * object)
{
	if(! object)
		return;

	unsigned int i;
	unsigned int oSize;

	// TODO: Remove entries that contain NULL somewhere!
	m_handles[object->getId()] = NULL;

	// objects
	switch(object->getType())
	{
	case M_OBJECT3D_ENTITY:
		{
			// entities
			oSize = m_entities.size();
			for(i=0; i<oSize; i++){
				Object3d * obj = m_entities[i];
				if(obj == object){
					m_entities.erase(m_entities.begin() + i);
					break;
				}
			}
		}
		break;

	case M_OBJECT3D_CAMERA:
		{
			// cameras
			oSize = m_cameras.size();
			for(i=0; i<oSize; i++){
				Object3d * obj = m_cameras[i];
				if(obj == object){
					m_cameras.erase(m_cameras.begin() + i);
					break;
				}
			}
		}
		break;

	case M_OBJECT3D_LIGHT:
		{
			// lights
			oSize = m_lights.size();
			for(i=0; i<oSize; i++){
				Object3d * obj = m_lights[i];
				if(obj == object){
					m_lights.erase(m_lights.begin() + i);
					break;
				}
			}
		}
		break;

	case M_OBJECT3D_SOUND:
		{
			// sounds
			oSize = m_sounds.size();
			for(i=0; i<oSize; i++){
				Object3d * obj = m_sounds[i];
				if(obj == object){
					m_sounds.erase(m_sounds.begin() + i);
					break;
				}
			}
		}
		break;

	case M_OBJECT3D_TEXT:
		{
			// sounds
			oSize = m_texts.size();
			for(i=0; i<oSize; i++){
				Object3d * obj = m_texts[i];
				if(obj == object){
					m_texts.erase(m_texts.begin() + i);
					break;
				}
			}
		}
		break;
	}

	// objects pointer
	oSize = m_objects.size();
	for(i=0; i<oSize; i++){
		Object3d * obj = m_objects[i];
		if(obj == object)
		{
			obj->unLink();
			obj->unlinkChildren();

			m_objects.erase(m_objects.begin() + i);
			SAFE_DELETE(obj);
			return;
		}
	}
}

bool Scene::getObjectIndex(const char * name, unsigned int * id)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		if(strcmp(name, m_objects[i]->getName()) == 0)
		{
			(*id) = i;
			return true;
		}
	}

	return false;
}

Object3d * Scene::getObjectByName(const char * name)
{
	unsigned int id;
	if(getObjectIndex(name, &id)){
		return m_objects[id];
	}

	return NULL;
}

OLight * Scene::getLightByName(const char * name)
{
	unsigned int i;
	unsigned int size = getLightsNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_lights[i]->getName()) == 0)
			return m_lights[i];
	}

	return NULL;
}

OCamera * Scene::getCameraByName(const char * name)
{
	unsigned int i;
	unsigned int size = getCamerasNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_cameras[i]->getName()) == 0)
			return m_cameras[i];
	}

	return NULL;
}

OEntity * Scene::getEntityByName(const char * name)
{
	unsigned int i;
	unsigned int size = getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_entities[i]->getName()) == 0)
			return m_entities[i];
	}

	return NULL;
}

OSound * Scene::getSoundByName(const char * name)
{
	unsigned int i;
	unsigned int size = getSoundsNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_sounds[i]->getName()) == 0)
			return m_sounds[i];
	}

	return NULL;
}

OText * Scene::getTextByName(const char * name)
{
	unsigned int i;
	unsigned int size = getTextsNumber();
	for(i=0; i<size; i++)
	{
		if(strcmp(name, m_texts[i]->getName()) == 0)
			return m_texts[i];
	}

	return NULL;
}

void Scene::begin(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	ScriptContext * scriptContext = engine->getScriptContext();

	updateObjectsMatrices();
	playLoopSounds();
	preparePhysics();

	// run script
	if(scriptContext)
		scriptContext->runScript(getScriptFilename());
}

void Scene::end(void)
{
	stopAllSounds();
}

void Scene::updatePhysics(void)
{
	PhysicsContext * physics = NeoEngine::getInstance()->getPhysicsContext();
	if(! physics)
		return;

	unsigned int i;
	unsigned int size = getEntitiesNumber();
	for(i=0; i<size; i++)
	{
		OEntity * entity = getEntityByIndex(i);
		if(! entity->isActive())
			continue;

		PhysicsProperties * phyProps = entity->getPhysicsProperties();
		if(! phyProps)
			continue;

		if(phyProps->getCollisionObjectId() > 0)
		{
			Object3d * parent = entity->getParent();
			if(parent && phyProps->isGhost())
			{
				Vector3 euler = entity->getTransformedRotation();
				physics->setObjectTransform(
					phyProps->getCollisionObjectId(),
					entity->getTransformedPosition(),
					Quaternion(euler.x, euler.y, euler.z)
				);
			}
			else if(entity->needToUpdate())
			{
				physics->setObjectTransform(
					phyProps->getCollisionObjectId(),
					entity->getPosition(),
					entity->getRotation()
				);
			}
		}
	}

	physics->setWorldGravity(m_gravity);
	physics->updateSimulation();

	for(i=0; i<size; i++)
	{
		OEntity * entity = getEntityByIndex(i);
		if(! entity->isActive())
			continue;

		PhysicsProperties * phyProps = entity->getPhysicsProperties();
		if(phyProps)
		{
			if((phyProps->getCollisionObjectId() > 0) && (! phyProps->isGhost()))
			{
				Vector3 position = entity->getPosition();
				Quaternion rotation = entity->getRotation();

				physics->getObjectTransform(phyProps->getCollisionObjectId(), &position, &rotation);

				entity->setPosition(position);
				entity->setRotation(rotation);
			}
		}
	}
}

void Scene::updateObjectsBehaviors(void)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		Object3d * object = getObjectByIndex(i);
		if(! object->isActive())
			continue;

		object->updateBehaviors();
	}
}

void Scene::drawObjectsBehaviors(void)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		Object3d * object = getObjectByIndex(i);
		if(! object->isActive())
			continue;

		object->drawBehaviors();
	}
}

void Scene::updateObjectsMatrices(void)
{
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		Object3d * object = getObjectByIndex(i);
		if(! object->isActive())
			continue;

		if(! object->hasParent())
		{
			if(object->needToUpdate())
				object->computeLocalMatrix();
			object->computeChildrenMatrices();
		}
	}
}

void Scene::update(void)
{
	// update objects
	unsigned int i;
	unsigned int oSize = getObjectsNumber();
	for(i=0; i<oSize; i++)
	{
		Object3d * object = getObjectByIndex(i);
		if(object->isActive())
			object->update();
	}

	// update current frame
	m_currentFrame++;
}

void Scene::playLoopSounds()
{
	unsigned int i;

	// sound sources
	unsigned int sSize = getSoundsNumber();
	for(i=0; i<sSize; i++)
	{
		if(m_sounds[i]->isActive() && m_sounds[i]->isLooping())
			m_sounds[i]->play();
	}
}

void Scene::stopAllSounds()
{
	unsigned int i;

	// sound sources
	unsigned int sSize = getSoundsNumber();
	for(i=0; i<sSize; i++)
		m_sounds[i]->stop();
}

void Scene::draw(OCamera * camera)
{
	NeoEngine * engine = NeoEngine::getInstance();
	Renderer * renderer = engine->getRenderer();

	if(renderer)
    {
		renderer->drawScene(this, camera);
    }
}
