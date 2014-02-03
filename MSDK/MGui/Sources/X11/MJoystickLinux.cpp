/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MJoystickLinux.cpp
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

#include "../../Includes/X11/MJoystickLinux.h"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

MJoystickLinux::MJoystickLinux(const char* joydev)
{
	if(joydev == NULL)
	{
		return;
	}
	
	m_numAxes = 0;
	m_numButtons = 0;
	m_axis = NULL;
	m_button = NULL;
	m_dAxis = NULL;
	
	if((m_file = open(joydev, O_RDONLY)) == -1)
	{
		return;
	}
	
	ioctl(m_file, JSIOCGAXES, &m_numAxes);
	ioctl(m_file, JSIOCGBUTTONS, &m_numButtons);
	ioctl(m_file, JSIOCGNAME(80), &m_joystickName);
	
	m_axis = (int*) calloc(m_numAxes, sizeof(int));
	m_dAxis = (float*) calloc(m_numAxes, sizeof(float));
	m_button = (char*) calloc(m_numButtons, sizeof(char));
	
	fcntl(m_file, F_SETFL, O_NONBLOCK);
}

MJoystickLinux::~MJoystickLinux()
{
	
}

int MJoystickLinux::getAxis(int idx)
{
	if(idx < m_numAxes)
	{
		return m_axis[idx];
	}
	
	return 0;
}

float MJoystickLinux::getDeltaAxis(int idx)
{
	if(idx < m_numAxes)
	{
		return m_dAxis[idx];
	}
	
	return 0;
}

bool MJoystickLinux::getButton(int idx)
{
	if(idx < m_numButtons)
	{
		return (bool) m_button[idx];
	}
	
	return false;
}

void MJoystickLinux::updateData()
{
	if(m_file == -1)
		return;
	
	fsync(m_file);
	read(m_file, &m_js, sizeof(struct js_event));
	
	switch(m_js.type & ~JS_EVENT_INIT)
	{
		case JS_EVENT_AXIS:
			m_axis[m_js.number] = m_js.value;
			m_dAxis[m_js.number] = m_js.value;
			m_dAxis[m_js.number] /= (float) JOY_AXIS_MAX;
			break;
		case JS_EVENT_BUTTON:
			m_button[m_js.number] = m_js.value;
			break;
	}
	
	
}

