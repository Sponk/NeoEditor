/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOEntity.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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


#include "../Includes/MEngine.h"


MPhysicsConstraint::MPhysicsConstraint(void):
constraintId(0),
parentObjectId(0),
lowerAngularLimit(-180),
upperAngularLimit(180),
disableParentCollision(false),
parentName("...")
{}

MPhysicsConstraint::MPhysicsConstraint(const MPhysicsConstraint & constraint):
constraintId(0),
parentObjectId(0),
parentName(constraint.parentName),
pivot(constraint.pivot),
lowerLinearLimit(constraint.lowerLinearLimit),
upperLinearLimit(constraint.upperLinearLimit),
lowerAngularLimit(constraint.lowerAngularLimit),
upperAngularLimit(constraint.upperAngularLimit),
disableParentCollision(constraint.disableParentCollision)
{}


MPhysicsProperties::MPhysicsProperties(void):
	m_constraint(NULL),
	m_ghost(false),
	m_shapeId(0),
	m_collisionObjectId(0),
	m_collisionShape(M_COLLISION_SHAPE_BOX),
	m_friction(0.5f),
	m_mass(0),
	m_restitution(0.0f),
	m_linearDamping(0.01f),
	m_angularDamping(0.01f),
	m_angularFactor(1),
	m_linearFactor(1, 1, 1)
{}

MPhysicsProperties::MPhysicsProperties(const MPhysicsProperties & physicsProperties):
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
	if(physicsProperties.m_constraint)
		m_constraint = new MPhysicsConstraint(*physicsProperties.m_constraint);
}

MPhysicsProperties::~MPhysicsProperties(void)
{
	deleteConstraint();
}

void MPhysicsProperties::deleteConstraint(void)
{
	SAFE_DELETE(m_constraint);
}

MPhysicsConstraint * MPhysicsProperties::createConstraint(void)
{
	deleteConstraint();
	m_constraint = new MPhysicsConstraint();
	return m_constraint;
}


MOEntity::MOEntity(MMeshRef * meshRef):
MObject3d(),
m_isInvisible(false),
m_animationSpeed(1),
m_currentFrame(0),
m_currentLoop(0),
m_animationId(0),
m_physicsProperties(NULL)
{
	setMeshRef(meshRef);
}

MOEntity::MOEntity(const MOEntity & entity):
MObject3d(entity),
m_isInvisible(entity.m_isInvisible),
m_physicsProperties(NULL),
m_animationSpeed(entity.m_animationSpeed),
m_currentFrame(entity.m_currentFrame),
m_currentLoop(entity.m_currentLoop),
m_animationId(entity.m_animationId)
{
	setMeshRef(entity.m_meshRef);
	if(entity.m_physicsProperties)
		m_physicsProperties = new MPhysicsProperties(*entity.m_physicsProperties);
}

MOEntity::~MOEntity(void)
{
	deletePhysicsProperties();
	MObject3d::clearObject3d();
}

void MOEntity::changeAnimation(unsigned int animationId)
{
	MMesh * mesh = getMesh();
	if(! mesh)
	{
		m_animationId = animationId;
		return;
	}

	if(animationId >= mesh->getAnimsRangesNumber())
		return;

	bool init = false;
	
	// change anim
	if(animationId != m_animationId)
	{
		m_animationId = animationId;
		init = true;
	}
	else
	{
		// or restart stopped anim
		MAnimRange * animRange = &mesh->getAnimsRanges()[m_animationId];
		if(m_currentLoop == animRange->loops)
			init = true;
	}
	
	if(init)
	{
		MAnimRange * animRange = &mesh->getAnimsRanges()[m_animationId];
		
		if(m_animationSpeed >= 0)
			m_currentFrame = animRange->start;
		else
			m_currentFrame = animRange->end;
		
		m_currentLoop = 0;
	}
}

void MOEntity::deletePhysicsProperties(void)
{
	SAFE_DELETE(m_physicsProperties);
}

MPhysicsProperties * MOEntity::createPhysicsProperties(void)
{
	deletePhysicsProperties();
	MPhysicsProperties * physicsProperties = new MPhysicsProperties();
	m_physicsProperties = physicsProperties;
	return physicsProperties;
}

void MOEntity::setActive(bool active)
{
	m_isActive = active;
}

void MOEntity::setMeshRef(MMeshRef * meshRef)
{
	// set mesh ref
	m_meshRef = meshRef;
	if(meshRef)
	{
		MMesh * mesh = meshRef->getMesh();
		if(mesh)
		{
			// create nounding box
			m_boundingBox = (*mesh->getBoundingBox());
		}
	}
}

MMesh * MOEntity::getMesh(void)
{
	if(! m_meshRef)
		return NULL;

	return m_meshRef->getMesh();
}

void MOEntity::updateVisibility(MOCamera * camera)
{
	if(m_isInvisible){
		setVisible(false);
		return;
	}

	MFrustum * frustum = camera->getFrustum();

	MVector3 * min = &m_boundingBox.min;
	MVector3 * max = &m_boundingBox.max;

	MVector3 points[8] = {
		getTransformedVector(MVector3(min->x, min->y, min->z)),
		getTransformedVector(MVector3(min->x, max->y, min->z)),
		getTransformedVector(MVector3(max->x, max->y, min->z)),
		getTransformedVector(MVector3(max->x, min->y, min->z)),
		getTransformedVector(MVector3(min->x, min->y, max->z)),
		getTransformedVector(MVector3(min->x, max->y, max->z)),
		getTransformedVector(MVector3(max->x, max->y, max->z)),
		getTransformedVector(MVector3(max->x, min->y, max->z))
	};

	// is box in frustum
	setVisible(frustum->isVolumePointsVisible(points, 8));
}

bool MOEntity::isAnimationOver(void)
{
	MMesh * mesh = getMesh();
	if(mesh)
	{
		if(mesh->getAnimsRangesNumber() > 0)
		{
			MAnimRange * animRange = &mesh->getAnimsRanges()[m_animationId];
			
			// end loops
			if(animRange->loops >= 0 && m_currentLoop == animRange->loops)
				return true;
			
			// next frame
			float nextFrame = m_currentFrame + m_animationSpeed;
			
			if(m_animationSpeed > 0)
				return (nextFrame >= (animRange->end + 1));
			else
				return (nextFrame <= animRange->start);
		}
	}
	
	return false;
}

void MOEntity::update(void)
{	
	MMesh * mesh = getMesh();
	if(mesh)
	{
		// anim
		if(mesh->getAnimsRangesNumber() > 0 && m_animationSpeed > 0)
		{
			MAnimRange * animRange = &mesh->getAnimsRanges()[m_animationId];

			// end loop
			if(animRange->loops >= 0 && m_currentLoop == animRange->loops)
				return;
			
			// next frame
			float nextFrame = m_currentFrame + m_animationSpeed;
		
			// update current frame
			if(animRange->loops < 0)
			{
				m_currentFrame = loopFloat(nextFrame, animRange->start, (animRange->end + 1));
			}
			else
			{
				if(isAnimationOver())
				{
					m_currentLoop++;
				}
				
				if(m_currentLoop < animRange->loops)
				{
					m_currentFrame = loopFloat(nextFrame, animRange->start, (animRange->end + 1));
				}
			}
		}
	}
}
