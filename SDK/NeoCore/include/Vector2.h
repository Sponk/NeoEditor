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


#ifndef __VECTOR2_H
#define __VECTOR2_H

namespace Neo
{
class NEO_CORE_EXPORT Vector2
{
public:

	float x;
	float y;

public:

	Vector2(void):
		x(0.0f), 
		y(0.0f)
	{}

	Vector2(float newX, float newY):
		x(newX), 
		y(newY)
	{}

	Vector2(const float value):
		x(value), 
		y(value)
	{}

	Vector2(const float * values):
		x((*values)), 
		y((*values)+1)
	{}

	Vector2(const Vector2 & vec):
		x(vec.x), 
		y(vec.y)
	{}

	Vector2(const Vector3 & vec);

public:

	Vector2 add(const Vector2 & vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}

	Vector2 substract(const Vector2 & vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}

	Vector2 multiply(const float value) const
	{
		return Vector2(x * value, y * value);
	}

	Vector2 divide(const float value) const
	{
		if (value == 0.0f) return Vector2(0.0f, 0.0f);
		return Vector2(x / value, y / value);
	}

	bool equals(const Vector2 & vec) const
	{
		return !((*this) == vec);
	}

	inline Vector2 operator + (const float value) const
	{	
		return Vector2(x + value, y + value);
	}

	inline Vector2 operator - (const float value) const
	{	
		return Vector2(x - value, y - value);
	}

	inline Vector2 operator + (const Vector2 & vec) const
	{	
		return Vector2(x + vec.x, y + vec.y);
	}

	inline Vector2 operator - (const Vector2 & vec) const
	{	
		return Vector2(x - vec.x, y - vec.y);
	}

	inline Vector2 operator * (const float value) const
	{	
		return Vector2(x * value, y * value);
	}
	
	inline Vector2 operator * (const Vector2 & vec) const
	{	
		return Vector2(x * vec.x, y * vec.y);
	}

	inline Vector2 operator / (const Vector2 & vec) const
	{	
		return Vector2(x / vec.x, y / vec.y);
	}

	inline Vector2 operator / (const float value) const
	{	
		if(value == 0.0f) return Vector2(0.0f, 0.0f);
		return Vector2(x / value, y / value);
	}

	friend inline Vector2 operator * (float factor, const Vector2 & vec) 
	{
		return vec * factor;
	}

	inline bool operator == (const Vector2 & vec) const
	{
		if(x == vec.x && y == vec.y){
			return true;
		}

		return false;
	}

	inline void operator = (const float value)
	{	
		x = value;	
		y = value;	
	}

	inline bool operator != (const Vector2 & vec) const
	{	
		return !((*this) == vec);
	}

	inline void operator += (const Vector2 & vec) 
	{	
		x += vec.x;
		y += vec.y;
	}

	inline void operator -= (const Vector2 & vec)
	{	
		x -= vec.x;	
		y -= vec.y;
	}

	inline void operator *= (const Vector2 & vec) 
	{	
		x *= vec.x;	
		y *= vec.y;
	}

	inline void operator /= (const Vector2 & vec) 
	{	
		x /= vec.x;	
		y /= vec.y;
	}

	inline void operator *= (const float value) 
	{	
		x *= value;	
		y *= value;
	}

	inline void operator /= (const float value) 
	{	
		if(value == 0.0f){
			x = 0.0f;
			y = 0.0f;
		}
		else
		{	
			x /= value; 
			y /= value;
		}
	}

	inline Vector2 operator - (void) const 
	{
		return Vector2(-x, -y);
	}

	inline Vector2 operator + (void) const 
	{
		return *this;
	}

	inline operator float* () const 
	{
		return (float*) this;
	}

	inline operator const float* () const 
	{
		return (const float*) this;
	}

public:

	inline void set(float newX, float newY)
	{	
		x = newX;	
		y = newY;
	}
	
	inline void loadIdentity(void)
	{
		x = 0.0f;
		y = 0.0f;
	}
	
	void normalize();
	void rotate(const float angle);	

	Vector2 getNormalized() const;
	Vector2 getRotated(const float angle) const;	

	// return angle that vec needs to rotate
	inline float getAngleBetween(const Vector2 & vec)
	{
		float angle = (float)((atan2(y, x) - atan2(vec.y, vec.x))*RAD_TO_DEG);
		
		if(angle > 180)
			angle -= 360;
		if(angle < -180)
			angle += 360;
		
		return angle;
	}

	inline float getLength() const
	{	
		return sqrtf((x*x) + (y*y));
	}

	inline float getSquaredLength() const
	{	
		return ((x*x) + (y*y));
	}

	inline float dotProduct(const Vector2 & vec) const
	{	
		return (x*vec.x) + (y*vec.y);
	}

	inline Vector2 lerp(const Vector2 & vec, float factor) const
	{	
		return ((*this) * (1.0f - factor)) + (vec * factor);
	}
};
}

#endif
