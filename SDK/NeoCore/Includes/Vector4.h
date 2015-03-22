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


#ifndef __VECTOR4_H
#define __VECTOR4_H

namespace Neo
{
class NEO_CORE_EXPORT Vector4
{
public:

	float x;
	float y;
	float z;
	float w;

public:

	Vector4(void):
		x(0.0f),
		y(0.0f),
		z(0.0f),
		w(0.0f)
	{}

	Vector4(float newX, float newY, float newZ, float newW):
		x(newX),
		y(newY),
		z(newZ),
		w(newW)
	{}

	Vector4(const float value):
		x(value),
		y(value),
		z(value),
		w(value)
	{}
	
	Vector4(const float * values):
		x(*(values)),
		y(*(values + 1)),
		z(*(values + 2)),
		w(*(values + 3))
	{}

	Vector4(const Vector4 & vec):
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(vec.w)
	{}

	Vector4(const Vector3 & vec):
		x(vec.x),
		y(vec.y),
		z(vec.z),
		w(1.0f)
	{}

	Vector4(const Color & color);

public:

	Vector4 operator + (const Vector4 & vec) const
	{
		return Vector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	Vector4 operator - (const Vector4 & vec) const
	{
		return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	Vector4 operator * (const float value) const
	{
		return Vector4(x * value, y * value, z * value, w * value);
	}

	Vector4 operator / (const float value) const
	{
		if(value == 0.0f) return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		return Vector4(x / value, y / value, z / value, w / value);
	}

	friend Vector4 operator * (float factor, const Vector4 & vec)
	{
		return vec * factor;
	}

	bool operator == (const Vector4 & vec) const
	{
		if(x == vec.x && y == vec.y && z == vec.z && w == vec.w){
			return true;
		}

		return false;
	}

	bool operator != (const Vector4 & vec) const
	{
		return !((*this) == vec);
	}

	void operator += (const Vector4 & vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
	}

	void operator -= (const Vector4 & vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
	}

	void operator *= (const float value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
	}

	void operator /= (const float value)
	{
		if(value == 0.0f){
			return;
		}
		else
		{
			x /= value;
			y /= value;
			z /= value;
			w /= value;
		}
	}

	Vector4 operator - (void) const
	{
		return Vector4(-x, -y, -z, -w);
	}

	Vector4 operator + (void) const
	{
		return (*this);
	}

	operator float* () const
	{
		return (float*) this;
	}

	operator const float* () const
	{
		return (const float*) this;
	}

	operator Vector3();

public:

	void set(float newX, float newY, float newZ, float newW)
	{
		x = newX;
		y = newY;
		z = newZ;
		w = newW;
	}

	void loadIdentity(void)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	float dotProduct(const Vector4 & vec)
	{
		return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
	}

	Vector4 lerp(const Vector4 & vec, float factor) const
	{
		return ((*this) * (1.0f - factor)) + (vec * factor);
	}
};
}

#endif
