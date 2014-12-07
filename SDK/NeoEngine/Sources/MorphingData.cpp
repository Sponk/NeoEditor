/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMorphingData.cpp
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


#include "../Includes/NeoEngine.h"

using namespace Neo;

MorphingPoint::MorphingPoint(void):
m_vertexId(0)
{}

MorphingPoint::~MorphingPoint(void)
{}


MorphingPose::MorphingPose(void):
m_factor(0),
m_pointsNumber(0),
m_points(NULL)
{}

MorphingPose::~MorphingPose(void)
{
	clearPoints();
}

MorphingPoint * MorphingPose::allocPoints(unsigned int size)
{
	clearPoints();
    if(size == 0)
		return NULL;
	m_pointsNumber = size;
	m_points = new MorphingPoint[size];
	return m_points;
}

void MorphingPose::clearPoints(void)
{
	m_pointsNumber = 0;
	SAFE_DELETE_ARRAY(m_points);
}

MorphingData::MorphingData(void):
m_posesNumber(0),
m_poses(NULL)
{}

MorphingData::~MorphingData(void)
{
	clearPoses();
}

void MorphingData::allocPoses(unsigned int size)
{
	clearPoses();
	if(size == 0)
		return;
	m_poses = (MorphingPose **) new int[size];
}

void MorphingData::clearPoses(void)
{
	unsigned int i;
	for(i=0; i<m_posesNumber; i++)
		SAFE_DELETE(m_poses[i]);

	m_posesNumber = 0;
	SAFE_DELETE_ARRAY(m_poses);
}

void MorphingData::addPose(MorphingPose * pose)
{
	m_poses[m_posesNumber] = pose;
	m_posesNumber++;
}
