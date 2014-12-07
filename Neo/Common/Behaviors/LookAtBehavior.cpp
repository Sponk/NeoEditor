/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBLookAt.cpp
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
#include <LookAtBehavior.h>

using namespace Neo;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

LookAtBehavior::LookAtBehavior(Object3d * parentObject):
Behavior(parentObject),
m_targetName("none")
{}

LookAtBehavior::LookAtBehavior(LookAtBehavior & behavior, Object3d * parentObject):
Behavior(parentObject),
m_targetName(behavior.m_targetName)
{}

LookAtBehavior::~LookAtBehavior(void)
{}

void LookAtBehavior::destroy(void)
{
	delete this;
}

Behavior * LookAtBehavior::getNew(Object3d * parentObject)
{
    return new LookAtBehavior(parentObject);
}

Behavior * LookAtBehavior::getCopy(Object3d * parentObject)
{
    return new LookAtBehavior(*this, parentObject);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int LookAtBehavior::getVariablesNumber(void){
	return 1;
}

NeoVariable LookAtBehavior::getVariable(unsigned int id)
{
	switch(id)
	{
	default:
		return NeoVariable("NULL", NULL, M_VARIABLE_NULL);
	case 0:
		return NeoVariable("target", &m_targetName, M_VARIABLE_STRING);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Events
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void LookAtBehavior::update(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
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

	// direction
	MVector3 direction = object->getTransformedPosition() - parent->getTransformedPosition();
	if(direction.x == 0 && direction.y == 0 && direction.z == 0)
		return;

	float angle;
	float roll;

	MVector3 axis;

	// compute initial roll
	MVector3 ZAxis = parent->getInverseRotatedVector(MVector3(0, 0, 1)).getNormalized();
	ZAxis.z = 0;
	ZAxis.normalize();

	if(ZAxis.x == 0 && ZAxis.y == 0)
	{
		MVector3 YAxis = parent->getInverseRotatedVector(MVector3(0, 1, 0)).getNormalized();
		YAxis.z = 0;
		YAxis.normalize();

		axis = MVector3(0, 1, 0).crossProduct(YAxis);
		roll = acosf(MVector3(0, 1, 0).dotProduct(YAxis));

		if(MVector3(0, 0, 1).dotProduct(axis) < 0)
			roll = -roll;
	}
	else
	{
		axis = MVector3(0, 1, 0).crossProduct(ZAxis);
		roll = acosf(MVector3(0, 1, 0).dotProduct(ZAxis));

		if(MVector3(0, 0, 1).dotProduct(axis) < 0)
			roll = -roll;
	}

	if(roll < 0.001f && roll > -0.001f) roll = 0;

	// look-at
	MVector3 cameraAxis = MVector3(0, 0, -1);

	axis = cameraAxis.crossProduct(direction);
	angle = acosf(cameraAxis.dotProduct(direction.getNormalized()));

	parent->setAxisAngleRotation(axis, (float)(angle * RAD_TO_DEG));
	parent->updateMatrix();

	// set roll
	ZAxis = parent->getInverseRotatedVector(MVector3(0, 0, 1)).getNormalized();;
	ZAxis.z = 0;
	ZAxis.normalize();

	if(ZAxis.x == 0 && ZAxis.y == 0)
	{
		parent->addAxisAngleRotation(MVector3(0, 0, 1), (float)(-roll*RAD_TO_DEG));
	}
	else
	{
		axis = MVector3(0, 1, 0).crossProduct(ZAxis);
		angle = acosf(MVector3(0, 1, 0).dotProduct(ZAxis));
		if(angle < 0.001f && angle > -0.001f) angle = 0;

		if(MVector3(0, 0, 1).dotProduct(axis) < 0)
			angle = -angle;

		parent->addAxisAngleRotation(MVector3(0, 0, 1), (float)((angle-roll)*RAD_TO_DEG));
	}

}
