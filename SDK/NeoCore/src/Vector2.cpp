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


#include <NeoCore.h>

using namespace Neo;

Vector2::Vector2(const Vector3 & vec):
		x(vec.x), 
		y(vec.y)
	{}

void Vector2::normalize()
{
	float length;
	float factor;

	length = getLength();

	if(length == 1 || length == 0){
		return;
	}

	factor = 1.0f / length;
	x *= factor;
	y *= factor;
}

void Vector2::rotate(const float angle)
{
	if(angle == 0.0){
		return;
	}

	float a = (float)(angle * DEG_TO_RAD);
	float sinAngle = sinf(a);
	float cosAngle = cosf(a);
	
	float rx = x * cosAngle - y * sinAngle;
	float ry = x * sinAngle + y * cosAngle;

	x = rx;
	y = ry;
}

Vector2 Vector2::getRotated(const float angle) const
{
	if(angle == 0.0){
		return (*this);
	}

	float a = (float)(angle * DEG_TO_RAD);
	float sinAngle = sinf(a);
	float cosAngle = cosf(a);
	
	return Vector2(
		x * cosAngle - y * sinAngle,
		x * sinAngle + y * cosAngle
	);
}

Vector2 Vector2::getNormalized() const
{
	Vector2 result(*this);
	result.normalize();

	return result;
}
