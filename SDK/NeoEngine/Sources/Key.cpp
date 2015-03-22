/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// Key.cpp
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

Key::Key(void):
m_t(0),
m_data(NULL)
{}

Key::~Key(void)
{
	SAFE_FREE(m_data);
}

float * Key::createFloatData(void)
{
	SAFE_FREE(m_data);
	float * value = new float;
	m_data = value;
	return value;
}

Vector2 * Key::createVector2Data(void)
{
	SAFE_FREE(m_data);
	Vector2 * vec2 = new Vector2();
	m_data = vec2;
	return vec2;
}

Vector3 * Key::createVector3Data(void)
{
	SAFE_FREE(m_data);
	Vector3 * vec3 = new Vector3();
	m_data = vec3;
	return vec3;
}

Vector4 * Key::createVector4Data(void)
{
	SAFE_FREE(m_data);
	Vector4 * vec4 = new Vector4();
	m_data = vec4;
	return vec4;
}

Quaternion * Key::createQuaternionData(void)
{
	SAFE_FREE(m_data);
	Quaternion * quat = new Quaternion();
	m_data = quat;
	return quat;
}
