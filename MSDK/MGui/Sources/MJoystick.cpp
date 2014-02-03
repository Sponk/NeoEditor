/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MJoystick.cpp
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


#include "../Includes/MJoystick.h"


MJoystick::MJoystick(void)
{
	for(int i=0; i<8; i++)
		m_keys[i] = 0;

	m_x = m_y = m_z = m_r = m_u = m_v = 0;
}

void MJoystick::downKey(unsigned int key)
{
	m_keys[key] = 1;
}

void MJoystick::upKey(unsigned int key)
{
	m_keys[key] = 3;
}

bool MJoystick::onKeyDown(unsigned int key)
{
	return (m_keys[key] == 1);
}

bool MJoystick::onKeyUp(unsigned int key)
{
	return (m_keys[key] == 3);
}

bool MJoystick::isKeyPressed(unsigned int key)
{
	return ((m_keys[key] == 1) || (m_keys[key] == 2));
}

void MJoystick::flush(void)
{
	for(int i=0; i<8; i++)
	{
		if(m_keys[i] == 1)
			m_keys[i] = 2;
		else if(m_keys[i] == 3)
			m_keys[i] = 0;
	}
}