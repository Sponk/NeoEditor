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

SkinPoint::SkinPoint(void):
m_bonesNumber(0),
m_bonesIds(NULL),
m_bonesWeights(NULL),
m_vertexId(0)
{}

SkinPoint::~SkinPoint(void)
{
	clearBonesLinks();
}

void SkinPoint::clearBonesLinks(void)
{
	m_bonesNumber = 0;
	SAFE_DELETE_ARRAY(m_bonesIds);
	SAFE_DELETE_ARRAY(m_bonesWeights);
}

bool SkinPoint::allocateBonesLinks(unsigned int size)
{
	clearBonesLinks();
	if(size == 0)
		return false;

	m_bonesNumber = size;
	m_bonesIds = new unsigned short[size];
	m_bonesWeights = new float[size];
	return true;
}


SkinData::SkinData(void):
m_pointsNumber(0),
m_points(NULL)
{}

SkinData::~SkinData(void)
{
	clearPoints();
}

SkinPoint * SkinData::allocPoints(unsigned int size)
{
	clearPoints();
	m_pointsNumber = size;
	m_points = new SkinPoint[m_pointsNumber];
	return m_points;
}

void SkinData::clearPoints(void)
{
	m_pointsNumber = 0;
	SAFE_DELETE_ARRAY(m_points);
}
