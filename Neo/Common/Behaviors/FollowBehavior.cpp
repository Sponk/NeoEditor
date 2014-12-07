/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBFollow.cpp
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


#include <NeoEngine.h>
#include <FollowBehavior.h>

using namespace Neo;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

FollowBehavior::FollowBehavior(Object3d * parentObject):
Behavior(parentObject),
m_local(false),
m_delay(10),
m_targetName("none")
{}

FollowBehavior::FollowBehavior(FollowBehavior & behavior, Object3d * parentObject):
Behavior(parentObject),
m_local(behavior.m_local),
m_delay(behavior.m_delay),
m_offset(behavior.m_offset),
m_targetName(behavior.m_targetName)
{}

FollowBehavior::~FollowBehavior(void)
{}

void FollowBehavior::destroy(void)
{
	delete this;
}

Behavior * FollowBehavior::getNew(Object3d * parentObject)
{
	return new FollowBehavior(parentObject);
}

Behavior * FollowBehavior::getCopy(Object3d * parentObject)
{
	return new FollowBehavior(*this, parentObject);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int FollowBehavior::getVariablesNumber(void){
	return 4;
}

NeoVariable FollowBehavior::getVariable(unsigned int id)
{
	switch(id)
	{
	default:
		return NeoVariable("NULL", NULL, M_VARIABLE_NULL);
	case 0:
		return NeoVariable("target", &m_targetName, M_VARIABLE_STRING);
	case 1:
		return NeoVariable("delay", &m_delay, M_VARIABLE_FLOAT);
	case 2:
		return NeoVariable("offset", &m_offset, M_VARIABLE_VEC3);
	case 3:
		return NeoVariable("local", &m_local, M_VARIABLE_BOOL);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Events
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void FollowBehavior::update(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	NeoGame * game = engine->getGame();
	Level * level = engine->getLevel();
	Scene * scene = level->getCurrentScene();

	Object3d * parent = getParentObject();

	const char * targetName = m_targetName.getSafeString();
	if(strcmp(targetName, "none") == 0)
		return;

	// target object
	Object3d * object = scene->getObjectByName(targetName);
	if(! object)
		return;

	// targetPos
	MVector3 offset = m_offset;
	if(m_local)
		offset = object->getRotatedVector(offset);

	float delay = MAX(1, m_delay);
	MVector3 direction = (object->getTransformedPosition() + offset) - parent->getPosition();

	if(parent->getType() == M_OBJECT3D_ENTITY)
	{
		OEntity * entity = (OEntity *)parent;
		MPhysicsProperties * phyProps = entity->getPhysicsProperties();
		if(phyProps)
		{
			if(! phyProps->isGhost())
			{
				if(game->isRunning())
				{
					MPhysicsContext * physics = engine->getPhysicsContext();
					physics->addCentralForce(phyProps->getCollisionObjectId(), (direction/delay)*phyProps->getMass());
				}
				return;
			}
		}
	}

	MVector3 position = parent->getPosition() + (direction / delay);
	parent->setPosition(position);
	parent->updateMatrix();
}
