/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMorphingData.h
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


#ifndef _M_MORPHING_DATA_H
#define _M_MORPHING_DATA_H

namespace Neo
{
// Buffer
class M_ENGINE_EXPORT MorphingPoint
{
public:

	MorphingPoint(void);
	~MorphingPoint(void);

private:

	// vertex id
	unsigned int m_vertexId;

	// dif vector
	Vector3 m_difVector;

public:

	// get vertex id
	inline unsigned int getVertexId(void){ return m_vertexId; }
};


// Pose
class M_ENGINE_EXPORT MorphingPose
{
public:

	MorphingPose(void);
	~MorphingPose(void);

private:

	// factor
	float m_factor;

	// points
	unsigned int m_pointsNumber;
	MorphingPoint * m_points;

public:
	
	// points
	void clearPoints(void);
	MorphingPoint * allocPoints(unsigned int size);
	inline unsigned int getPointsNumber(void){ return m_pointsNumber; }
	inline MorphingPoint * getPoint(unsigned int id){ return &m_points[id]; }
};


// Morphing Data
class M_ENGINE_EXPORT MorphingData
{
public:

	MorphingData(void);
	~MorphingData(void);

private:

	unsigned int m_posesNumber;
	MorphingPose ** m_poses;

public:
	
	void allocPoses(unsigned int size);
	void clearPoses(void);
	void addPose(MorphingPose * pose);
	inline unsigned int getPosesNumber(void){ return m_posesNumber; }
	inline MorphingPose * getPose(unsigned int id){ return m_poses[id]; }
};
}

#endif
