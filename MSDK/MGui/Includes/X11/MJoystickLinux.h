/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MJoystickLinux.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2013 Yannick Pflanzer <yp1995@live.de>
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

#ifndef MJOYSTICK_LINUX_H
#define MJOYSTICK_LINUX_H

#include <linux/joystick.h>

#define JOY1_DEV "/dev/input/js0"
#define JOY2_DEV "/dev/input/js1"

#define JOY_AXIS_MIN -32767
#define JOY_AXIS_MAX 32767

class MJoystickLinux
{
public:
	MJoystickLinux(const char* joydev);
	~MJoystickLinux();
	void updateData();

	int getAxis(int idx);
	float getDeltaAxis(int idx);
	bool getButton(int idx);

private:
	int m_file, m_numAxes, m_numButtons;
	int* m_axis;
	float* m_dAxis;
	
	char* m_button, m_joystickName[80];
	
	struct js_event m_js;
};

#endif
