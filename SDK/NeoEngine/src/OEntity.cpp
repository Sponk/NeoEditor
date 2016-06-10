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


#include <NeoEngine.h>
#include <NeoCore.h>

using namespace Neo;

PhysicsConstraint::PhysicsConstraint(void)
	:
	constraintId(0),
	parentObjectId(0),
	lowerAngularLimit(-180),
	upperAngularLimit(180),
	disableParentCollision(false),
	parentName("...")
{ }

PhysicsConstraint::PhysicsConstraint(const PhysicsConstraint& constraint)
	:
	constraintId(0),
	parentObjectId(0),
	parentName(constraint.parentName),
	pivot(constraint.pivot),
	lowerLinearLimit(constraint.lowerLinearLimit),
	upperLinearLimit(constraint.upperLinearLimit),
	lowerAngularLimit(constraint.lowerAngularLimit),
	upperAngularLimit(constraint.upperAngularLimit),
	disableParentCollision(constraint.disableParentCollision)
{ }

PhysicsProperties::PhysicsProperties(void)
	:
	m_constraint(NULL),
	m_ghost(false),
	m_shapeId(0),
	m_collisionObjectId(0),
	m_collisionShape(COLLISION_SHAPE_BOX),
	m_friction(0.5f),
	m_mass(0),
	m_restitution(0.0f),
	m_linearDamping(0.01f),
	m_angularDamping(0.01f),
	m_angularFactor(1),
	m_linearFactor(1, 1, 1)
{ }

PhysicsProperties::PhysicsProperties(const PhysicsProperties& physicsProperties)
	:
	m_constraint(NULL),
	m_ghost(physicsProperties.m_ghost),
	m_shapeId(physicsProperties.m_shapeId),
	m_collisionObjectId(0),
	m_collisionShape(physicsProperties.m_collisionShape),
	m_friction(physicsProperties.m_friction),
	m_mass(physicsProperties.m_mass),
	m_restitution(physicsProperties.m_restitution),
	m_linearDamping(physicsProperties.m_linearDamping),
	m_angularDamping(physicsProperties.m_angularDamping),
	m_angularFactor(physicsProperties.m_angularFactor),
	m_linearFactor(physicsProperties.m_linearFactor)
{
	if (physicsProperties.m_constraint)
		m_constraint = new PhysicsConstraint(*physicsProperties.m_constraint);
}

PhysicsProperties::~PhysicsProperties(void)
{
	deleteConstraint();
}

void PhysicsProperties::deleteConstraint(void)
{
	SAFE_DELETE(m_constraint);
}

PhysicsConstraint* PhysicsProperties::createConstraint(void)
{
	deleteConstraint();
	m_constraint = new PhysicsConstraint();
	return m_constraint;
}

OEntity::OEntity(MeshRef* meshRef)
	:
	Object3d(),
	m_isInvisible(false),
	m_animationSpeed(1),
	m_currentFrame(0),
	m_currentLoop(0),
	m_animationId(0),
	m_physicsProperties(NULL),
	m_hasShadow(true),
	m_isOccluder(false),
	m_wireframe(false),
    m_materials(NULL),
    m_textures(NULL)
{
	setMeshRef(meshRef);
}

OEntity::OEntity(const OEntity& entity)
	:
	Object3d(entity),
	m_isInvisible(entity.m_isInvisible),
	m_physicsProperties(NULL),
	m_animationSpeed(entity.m_animationSpeed),
	m_currentFrame(entity.m_currentFrame),
	m_currentLoop(entity.m_currentLoop),
	m_animationId(entity.m_animationId),
	m_hasShadow(entity.m_hasShadow),
	m_isOccluder(entity.m_isOccluder),
	m_wireframe(entity.m_wireframe),
    m_materials(NULL),
    m_textures(NULL)
{
	setMeshRef(entity.m_meshRef);
	if (entity.m_physicsProperties)
		m_physicsProperties = new PhysicsProperties(*entity.m_physicsProperties);

	for(int i = 0; i < getMaterialsNumber() && i < entity.m_numMaterials; i++)
	{
		Material* material = getMaterial(i);
		Material* originalMaterial = &entity.m_materials[i];

		material->setEmit(originalMaterial->getEmit());
		material->setSpecular(originalMaterial->getSpecular());
		material->setShininess(originalMaterial->getShininess());
		material->setBlendMode(originalMaterial->getBlendMode());
		material->setDiffuse(originalMaterial->getDiffuse());
		material->setOpacity(originalMaterial->getOpacity());
		material->setCustomValue(originalMaterial->getCustomValue());
		material->setCustomColor(originalMaterial->getCustomColor());
	}
}

OEntity::~OEntity(void)
{
	deletePhysicsProperties();
	Object3d::clearObject3d();
}

PhysicsProperties* OEntity::enablePhysics(Scene* scene)
{
	PhysicsContext* physics = NeoEngine::getInstance()->getPhysicsContext();
	PhysicsProperties* phys =
		(m_physicsProperties) ? m_physicsProperties : createPhysicsProperties();

	updateMatrix();
	scene->prepareCollisionShape(this);
	scene->prepareCollisionObject(this);

	return phys;
}

PhysicsProperties* OEntity::enablePhysics()
{
	return enablePhysics(
		NeoEngine::getInstance()->getLevel()->getCurrentScene());
}

bool OEntity::isColliding(OEntity* entity)
{
	PhysicsContext* physics = NeoEngine::getInstance()->getPhysicsContext();
	return physics->isObjectsCollision(m_physicsProperties->getCollisionObjectId(),
									   entity->getPhysicsProperties()->getCollisionObjectId());
}

bool OEntity::isColliding()
{
	PhysicsContext* physics = NeoEngine::getInstance()->getPhysicsContext();
	return MIN(1, physics->isObjectInCollision(
		m_physicsProperties->getCollisionObjectId()));
}

void OEntity::changeAnimation(unsigned int animationId)
{
	Mesh* mesh = getMesh();
	if (!mesh)
	{
		m_animationId = animationId;
		return;
	}

	if (animationId >= mesh->getAnimsRangesNumber())
		return;

	bool init = false;

	// change anim
	if (animationId != m_animationId)
	{
		m_animationId = animationId;
		init = true;
	}
	else
	{
		// or restart stopped anim
		AnimRange* animRange = &mesh->getAnimsRanges()[m_animationId];
		if (m_currentLoop == animRange->loops)
			init = true;
	}

	if (init)
	{
		AnimRange* animRange = &mesh->getAnimsRanges()[m_animationId];

		if (m_animationSpeed >= 0)
			m_currentFrame = animRange->start;
		else
			m_currentFrame = animRange->end;

		m_currentLoop = 0;
	}
}

void OEntity::deletePhysicsProperties(void)
{
	SAFE_DELETE(m_physicsProperties);
}

PhysicsProperties* OEntity::createPhysicsProperties(void)
{
	deletePhysicsProperties();
	PhysicsProperties* physicsProperties = new PhysicsProperties();
	m_physicsProperties = physicsProperties;
	return physicsProperties;
}

void OEntity::setActive(bool active)
{
	m_isActive = active;
}

void OEntity::setMeshRef(MeshRef* meshRef)
{
	// set mesh ref
	m_meshRef = meshRef;
	if (meshRef)
	{
		Mesh* mesh = meshRef->getMesh();
		if (mesh)
		{
			// create bounding box
			m_boundingBox = (*mesh->getBoundingBox());
			m_hasTransparency = false;

			m_numMaterials = mesh->getMaterialsNumber();
			SAFE_DELETE_ARRAY(m_materials);

			m_materials = new Material[m_numMaterials];
			for (int i = 0; i < m_numMaterials; i++)
			{
				m_materials[i] = *mesh->getMaterial(i);
			}            

            SAFE_DELETE_ARRAY(m_textures);
            m_numTextures = mesh->getTexturesNumber();
            m_textures = new Texture*[m_numTextures];

            for(int i = 0; i < m_numTextures; i++)
            {
                m_textures[i] = mesh->getTexture(i);
            }

			for (int i = 0; i < mesh->getSubMeshsNumber(); i++)
			{
				if (mesh->getSubMeshs()[i].hasTransparency())
				{
					m_hasTransparency = true;
					break;
				}
			}
		}
	}
}

Mesh* OEntity::getMesh(void)
{
	if (!m_meshRef)
		return NULL;

	return m_meshRef->getMesh();
}

void OEntity::updateVisibility(OCamera* camera)
{
	if (m_isInvisible)
	{
		setVisible(false);
		return;
	}

	Frustum* frustum = camera->getFrustum();

	Vector3* min = &m_boundingBox.min;
	Vector3* max = &m_boundingBox.max;

	Vector3 points[8] = {
		getTransformedVector(Vector3(min->x, min->y, min->z)),
		getTransformedVector(Vector3(min->x, max->y, min->z)),
		getTransformedVector(Vector3(max->x, max->y, min->z)),
		getTransformedVector(Vector3(max->x, min->y, min->z)),
		getTransformedVector(Vector3(min->x, min->y, max->z)),
		getTransformedVector(Vector3(min->x, max->y, max->z)),
		getTransformedVector(Vector3(max->x, max->y, max->z)),
		getTransformedVector(Vector3(max->x, min->y, max->z))
	};

	// is box in frustum
	setVisible(frustum->isVolumePointsVisible(points, 8));
}

bool OEntity::isAnimationOver(void)
{
	Mesh* mesh = getMesh();
	if (mesh)
	{
		if (mesh->getAnimsRangesNumber() > 0)
		{
			AnimRange* animRange = &mesh->getAnimsRanges()[m_animationId];

			// end loops
			if (animRange->loops >= 0 && m_currentLoop == animRange->loops)
				return true;

			// next frame
			float nextFrame = m_currentFrame + m_animationSpeed;

			if (m_animationSpeed > 0)
				return (nextFrame >= (animRange->end + 1));
			else
				return (nextFrame <= animRange->start);
		}
	}

	return false;
}

void OEntity::update(void)
{
	Mesh* mesh = getMesh();
	if (mesh)
	{
		// anim
		if (mesh->getAnimsRangesNumber() > 0 && m_animationSpeed > 0)
		{
			AnimRange* animRange = &mesh->getAnimsRanges()[m_animationId];

			// end loop
			if (animRange->loops >= 0 && m_currentLoop == animRange->loops)
				return;

			// next frame
			float nextFrame = m_currentFrame + m_animationSpeed;

			// update current frame
			if (animRange->loops < 0)
			{
				m_currentFrame = loopFloat(nextFrame, animRange->start, (animRange->end + 1));
			}
			else
			{
				if (isAnimationOver())
				{
					m_currentLoop++;
				}

				if (m_currentLoop < animRange->loops)
				{
					m_currentFrame = loopFloat(nextFrame, animRange->start, (animRange->end + 1));
				}
			}
		}
	}
}

RayCastResult OEntity::castRay(Vector3& rayO, Vector3& rayD)
{
	RayCastResult result;

	Mesh* mesh = getMesh();
	mesh->updateBoundingBox();

	Box3d* box = mesh->getBoundingBox();
	Vector3 pos = getTransformedPosition();
	Matrix4x4 iMatrix = getMatrix()->getInverse();

	Vector3 localRayO = iMatrix * rayO;
	Vector3 localRayD = iMatrix * rayD;

	if (!isEdgeToBoxCollision(localRayO, localRayD, box->min, box->max))
	{
		result.hit = false;
		return result;
	}

	SubMesh* submeshes = mesh->getSubMeshs();
	for (int i = 0; i < mesh->getSubMeshsNumber(); i++)
	{
		Vector3* t1;
		Vector3* t2;
		Vector3* t3;
		Vector3* vertices = submeshes[i].getVertices();

		for (int j = 0; j < submeshes[i].getIndicesSize(); j += 3)
		{
			t1 = vertices + submeshes[i].getIndex(j);
			t2 = vertices + submeshes[i].getIndex(j + 1);
			t3 = vertices + submeshes[i].getIndex(j + 2);

			vertices = (Vector3*) submeshes[i].getVertices();
			result.hit = isEdgeTriangleIntersection(localRayO,
													localRayD,

													*t1,
													*t2,
													*t3,

													getTriangleNormal(
														*t1,
														*t2,
														*t3),

													&result.point);

			if (result.hit)
				return result;
		}
	}

	return result;
}
