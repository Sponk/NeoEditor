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

using namespace Neo;

// constructors
Armature::Armature(void):
m_bonesNumber(0),
m_bones(NULL)
{}

// destructor
Armature::~Armature(void)
{
	clearBones();
}

void Armature::clearBones(void)
{
	unsigned int i;
	for(i=0; i<m_bonesNumber; i++)
		SAFE_DELETE(m_bones[i]);

	SAFE_FREE(m_bones);
}

void Armature::allocBones(unsigned int size)
{
	clearBones();
	if(size == 0)
		return;

	m_bones = (OBone **) malloc(sizeof(void *)*size);
}

OBone * Armature::addNewBone(void)
{
	OBone * bone = new OBone();
	m_bones[m_bonesNumber] = bone;
	m_bonesNumber++;
	return bone;
}

OBone * Armature::getBoneByName(const char * name)
{
	unsigned int i;
	for(i=0; i<m_bonesNumber; i++) // scan bones
	{
		OBone * bone = getBone(i);
		if(strcmp(name, bone->getName()) == 0)
			return bone;
	}

	return NULL;
}

bool Armature::getBoneId(const char * boneName, unsigned int * bId)
{
	unsigned int i;

	// return if there is no bones
	if(m_bonesNumber < 1)
		return false;

	for(i=0; i<m_bonesNumber; i++)
	{
		OBone * iBone = m_bones[i];

		if(strcmp(boneName, iBone->getName()) == 0)
		{
			*bId = i;
			return true;
		}
	}

	return false;
}

void Armature::constructBonesInversePoseMatrix(void)
{
	unsigned int i;
	OBone * bone = NULL;

	processBonesLinking();

	// construct inverse pose matrix
	for(i=0; i<m_bonesNumber; i++)
	{
		bone = m_bones[i];
		(*bone->getInversePoseMatrix()) = bone->getMatrix()->getInverse();
	}
}

void Armature::updateBonesSkinMatrix(void)
{
	unsigned int i;
	OBone * bone = NULL;
	for(i=0; i<m_bonesNumber; i++)
	{
		bone = m_bones[i];
		(*bone->getSkinMatrix()) = (*bone->getMatrix()) * (*bone->getInversePoseMatrix());
	}
}

void Armature::processBonesLinking(void)
{
	unsigned int i;
	OBone * bone = NULL;

	// for all root bones
	for(i=0; i<m_bonesNumber; i++)
	{
		bone = m_bones[i];
		if(! bone->hasParent())
		{
			bone->computeLocalMatrix();
			bone->processChildrenLinking();
		}
	}
}
