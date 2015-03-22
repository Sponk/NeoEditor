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


#ifndef __QUATERNION_H
#define __QUATERNION_H

namespace Neo
{
class NEO_CORE_EXPORT Quaternion
{
public:

	float values[4];

public:

	Quaternion(void)
	{
		loadIdentity();
	}

	Quaternion(float val0, float val1, float val2, float val3)
	{
		values[0] = val0;
		values[1] = val1;
		values[2] = val2;
		values[3] = val3;
	}

	Quaternion(float angle, const Vector3 & axis)
	{
		setFromAngleAxis(angle, axis);
	}

	Quaternion(float xAngle, float yAngle, float zAngle)
	{
		setFromAngles(xAngle, yAngle, zAngle);
	}

	Quaternion(const Quaternion & q1, Quaternion q2, float interpolation)
	{
		slerp(q1, q2, interpolation);
	}

public:

	Quaternion operator * (const Quaternion & quat) const;

	void operator *= (const Quaternion & quat);

	Quaternion operator - (void) const
	{
		return Quaternion(-values[0], -values[1], -values[2], -values[3]);
	}

	inline bool operator == (const Quaternion & quat) const
	{
		if(values[0] == quat.values[0] && 
		   values[1] == quat.values[1] &&
		   values[2] == quat.values[2] &&
		   values[3] == quat.values[3])
			return true;

		return false;
	}

	inline bool operator != (const Quaternion & quat) const
	{	
		return !((*this) == quat);
	}

public:

	void normalize(void);
	void loadIdentity(void);
	void setFromAngles(float xAngle, float yAngle, float zAngle);
	void slerp(const Quaternion & q1, const Quaternion & q2, float interpolation);
	void invert(void);
	void setFromAngleAxis(float angle, const Vector3 & axis);
	void setFromVectors(const Vector3 & source, const Vector3 & destination);

	Vector3 getEulerAngles(void) const;

	float getAngle(void) const;
	Vector3 getAxis(void) const;
};
}
#endif
