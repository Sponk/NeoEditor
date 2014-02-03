/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MJoystick.h
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


#ifndef _M_JOYSTICK_H
#define _M_JOYSTICK_H


class MJoystick
{
public:

	MJoystick(void);

private:

	int m_keys[8];

	float m_x;
	float m_y;
	float m_z;
	float m_r;
	float m_u;
	float m_v;

public:

	void downKey(unsigned int key);
	void upKey(unsigned int key);

	bool onKeyDown(unsigned int key);
	bool onKeyUp(unsigned int key);
	bool isKeyPressed(unsigned int key);

	inline void setX(float x){ m_x = x; }
	inline void setY(float y){ m_y = y; }
	inline void setZ(float z){ m_z = z; }
	inline void setR(float r){ m_r = r; }
	inline void setU(float u){ m_u = u; }
	inline void setV(float v){ m_v = v; }

	inline float getX(void){ return m_x; }
	inline float getY(void){ return m_y; }
	inline float getZ(void){ return m_z; }
	inline float getR(void){ return m_r; }
	inline float getU(void){ return m_u; }
	inline float getV(void){ return m_v; }

	void flush(void);
};

#endif