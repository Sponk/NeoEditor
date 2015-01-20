/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MInput.cpp
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


#include "Input.h"

#include "MVector2.h"
#include "MVector3.h"

using namespace Neo;

Input::Input(void)
{
	// ASCII keys
	char name[2] = {0, 0};
	for(int i=65; i<=90; i++)
    {
		name[0] = i;
        createKey(name);
	}
    
    // create touch data
    for(int i=0; i<10; i++)
    {
        m_touches[i] = TouchData();
    }

	// keyboard keys
	createKey("BACKSPACE");
	createKey("TAB");
	createKey("ESCAPE");
	createKey("SPACE");
	createKey("DELETE");
	createKey("0");
	createKey("1");
	createKey("2");
	createKey("3");
	createKey("4");
	createKey("5");
	createKey("6");
	createKey("7");
	createKey("8");
	createKey("9");
	createKey("ENTER");
	createKey("UP");
	createKey("DOWN");
	createKey("LEFT");
	createKey("RIGHT");
	createKey("F1");
	createKey("F2");
	createKey("F3");
	createKey("F4");
	createKey("F5");
	createKey("F6");
	createKey("F7");
	createKey("F8");
	createKey("F9");
	createKey("F10");
	createKey("F11");
	createKey("F12");
	createKey("RSHIFT");
	createKey("LSHIFT");
	createKey("RCONTROL");
	createKey("LCONTROL");
	createKey("RALT");
	createKey("LALT");

	// mouse keys
	createKey("MOUSE_BUTTON_LEFT");
	createKey("MOUSE_BUTTON_MIDDLE");
	createKey("MOUSE_BUTTON_RIGHT");
	createKey("MOUSE_BUTTON_X1");
	createKey("MOUSE_BUTTON_X2");

	// joystick keys
	createKey("JOY1_BUTTON_A");
	createKey("JOY1_BUTTON_B");
	createKey("JOY1_BUTTON_X");
	createKey("JOY1_BUTTON_Y");
	createKey("JOY1_BUTTON_BACK");
	createKey("JOY1_BUTTON_GUIDE");
	createKey("JOY1_BUTTON_START");
	createKey("JOY1_BUTTON_LEFTSTICK");
	createKey("JOY1_BUTTON_RIGHTSTICK");
	createKey("JOY1_BUTTON_LEFTSHOULDER");
	createKey("JOY1_BUTTON_RIGHTSHOULDER");
	createKey("JOY1_BUTTON_DPADUP");
	createKey("JOY1_BUTTON_DPADDOWN");
	createKey("JOY1_BUTTON_DPADLEFT");
	createKey("JOY1_BUTTON_DPADRIGHT");

	createKey("JOY2_BUTTON_A");
	createKey("JOY2_BUTTON_B");
	createKey("JOY2_BUTTON_X");
	createKey("JOY2_BUTTON_Y");
	createKey("JOY2_BUTTON_BACK");
	createKey("JOY2_BUTTON_GUIDE");
	createKey("JOY2_BUTTON_START");
	createKey("JOY2_BUTTON_LEFTSTICK");
	createKey("JOY2_BUTTON_RIGHTSTICK");
	createKey("JOY2_BUTTON_LEFTSHOULDER");
	createKey("JOY2_BUTTON_RIGHTSHOULDER");
	createKey("JOY2_BUTTON_DPADUP");
	createKey("JOY2_BUTTON_DPADDOWN");
	createKey("JOY2_BUTTON_DPADLEFT");
	createKey("JOY2_BUTTON_DPADRIGHT");

	createKey("JOY3_BUTTON_A");
	createKey("JOY3_BUTTON_B");
	createKey("JOY3_BUTTON_X");
	createKey("JOY3_BUTTON_Y");
	createKey("JOY3_BUTTON_BACK");
	createKey("JOY3_BUTTON_GUIDE");
	createKey("JOY3_BUTTON_START");
	createKey("JOY3_BUTTON_LEFTSTICK");
	createKey("JOY3_BUTTON_RIGHTSTICK");
	createKey("JOY3_BUTTON_LEFTSHOULDER");
	createKey("JOY3_BUTTON_RIGHTSHOULDER");
	createKey("JOY3_BUTTON_DPADUP");
	createKey("JOY3_BUTTON_DPADDOWN");
	createKey("JOY3_BUTTON_DPADLEFT");
	createKey("JOY3_BUTTON_DPADRIGHT");

	createKey("JOY4_BUTTON_A");
	createKey("JOY4_BUTTON_B");
	createKey("JOY4_BUTTON_X");
	createKey("JOY4_BUTTON_Y");
	createKey("JOY4_BUTTON_BACK");
	createKey("JOY4_BUTTON_GUIDE");
	createKey("JOY4_BUTTON_START");
	createKey("JOY4_BUTTON_LEFTSTICK");
	createKey("JOY4_BUTTON_RIGHTSTICK");
	createKey("JOY4_BUTTON_LEFTSHOULDER");
	createKey("JOY4_BUTTON_RIGHTSHOULDER");
	createKey("JOY4_BUTTON_DPADUP");
	createKey("JOY4_BUTTON_DPADDOWN");
	createKey("JOY4_BUTTON_DPADLEFT");
	createKey("JOY4_BUTTON_DPADRIGHT");

	// axis
	createAxis("MOUSE_X");
	createAxis("MOUSE_Y");
	createAxis("MOUSE_WHEEL", 1);

	createAxis("JOY1_AXIS_LEFTX");
	createAxis("JOY1_AXIS_LEFTY");
	createAxis("JOY1_AXIS_RIGHTX");
	createAxis("JOY1_AXIS_RIGHTY");
	createAxis("JOY1_AXIS_TRIGGERLEFT");
	createAxis("JOY1_AXIS_TRIGGERRIGHT");

	createAxis("JOY2_AXIS_LEFTX");
	createAxis("JOY2_AXIS_LEFTY");
	createAxis("JOY2_AXIS_RIGHTX");
	createAxis("JOY2_AXIS_RIGHTY");
	createAxis("JOY2_AXIS_TRIGGERLEFT");
	createAxis("JOY2_AXIS_TRIGGERRIGHT");

	createAxis("JOY3_AXIS_LEFTX");
	createAxis("JOY3_AXIS_LEFTY");
	createAxis("JOY3_AXIS_RIGHTX");
	createAxis("JOY3_AXIS_RIGHTY");
	createAxis("JOY3_AXIS_TRIGGERLEFT");
	createAxis("JOY3_AXIS_TRIGGERRIGHT");

	createAxis("JOY4_AXIS_LEFTX");
	createAxis("JOY4_AXIS_LEFTY");
	createAxis("JOY4_AXIS_RIGHTX");
	createAxis("JOY4_AXIS_RIGHTY");
	createAxis("JOY4_AXIS_TRIGGERLEFT");
	createAxis("JOY4_AXIS_TRIGGERRIGHT");
}

Input::~Input(void)
{}

void Input::createKey(const char * name)
{
	if(name)
		m_keys[name] = 0;
}

void Input::createAxis(const char * name, bool flush)
{
	if(name)
	{
		m_axis[name] = 0;
		if(flush) m_axisToFlush.push_back(&m_axis[name]);
	}
}

void Input::createProperty(const char * name)
{
	if(name)
		m_props[name] = 0;
}

void Input::downKey(const char * name)
{
	map<string, int>::iterator iter = m_keys.find(name);
	if(iter != m_keys.end())
		iter->second = 1;
}

void Input::upKey(const char * name)
{
	map<string, int>::iterator iter = m_keys.find(name);
	if(iter != m_keys.end())
		iter->second = 3;
}

void Input::setAxis(const char * name, float axis)
{
	map<string, float>::iterator iter = m_axis.find(name);
	if(iter != m_axis.end())
		iter->second = axis;
}

void Input::setProperty(const char * name, int prop)
{
	map<string, int>::iterator iter = m_props.find(name);
	if(iter != m_props.end())
		iter->second = prop;
}

bool Input::isKeyPressed(const char * name)
{
	map<string, int>::iterator iter = m_keys.find(name);
	if(iter != m_keys.end())
		return (iter->second == 1 || iter->second == 2);

	return false;
}

bool Input::onKeyDown(const char * name)
{
	map<string, int>::iterator iter = m_keys.find(name);
	if(iter != m_keys.end())
		return (iter->second == 1);

	return false;
}

bool Input::onKeyUp(const char * name)
{
	map<string, int>::iterator iter = m_keys.find(name);
	if(iter != m_keys.end())
		return (iter->second == 3);

	return false;
}

float Input::getAxis(const char * name)
{
	map<string, float>::iterator iter = m_axis.find(name);
	if(iter != m_axis.end())
		return iter->second;

	return 0;
}

int Input::getProperty(const char * name)
{
	map<string, int>::iterator iter = m_props.find(name);
	if(iter != m_props.end())
		return iter->second;

	return 0;
}

void Input::beginTouch(int touchID, MVector2 touchPoint)
{
    map<int, TouchData>::iterator iter = m_touches.find(touchID);
    
    if(iter != m_touches.end())
    {
        TouchData* data = &(iter->second);
        data->touchPoint = touchPoint;
        data->lastTouchPoint = touchPoint;
        data->phase = M_TOUCH_BEGIN;
    }
}

void Input::updateTouch(int touchID, MVector2 touchPoint)
{
    map<int, TouchData>::iterator iter = m_touches.find(touchID);
    
    if(iter != m_touches.end())
    {
        TouchData* data = &(iter->second);
        data->lastTouchPoint = data->touchPoint;
        data->touchPoint = touchPoint;
        data->phase = M_TOUCH_UPDATE;
    }
}

void Input::endTouch(int touchID, MVector2 touchPoint)
{
    map<int, TouchData>::iterator iter = m_touches.find(touchID);
    
    if(iter != m_touches.end())
    {
        TouchData* data = &(iter->second);
        data->lastTouchPoint = data->touchPoint;
        data->touchPoint = touchPoint;
        data->phase = M_TOUCH_END;
    }
}

void Input::cancelTouch(int touchID, MVector2 touchPoint)
{
    map<int, TouchData>::iterator iter = m_touches.find(touchID);
    
    if(iter != m_touches.end())
    {
        TouchData* data = &(iter->second);
        data->lastTouchPoint = data->touchPoint;
        data->touchPoint = touchPoint;
        data->phase = M_TOUCH_CANCELLED;
    }
}

MVector2 Input::getTouchPosition(int touchID)
{
    map<int, TouchData>::iterator iter = m_touches.find(touchID);
    
    if(iter != m_touches.end())
    {
        TouchData* data = &(iter->second);
        
        if (data->phase != M_TOUCH_NONE)
        {
            return data->touchPoint;
        }
    }
    
    return MVector2(0.0f);
}

MVector2 Input::getLastTouchPosition(int touchID)
{
    map<int, TouchData>::iterator iter = m_touches.find(touchID);
    
    if(iter != m_touches.end())
    {
        TouchData* data = &(iter->second);
        if (data->phase != M_TOUCH_NONE)
        {
            return data->lastTouchPoint;
        }
    }
    
    return MVector2(0.0f);
}

M_TOUCH_PHASE Input::getTouchPhase(int touchID)
{
    map<int, TouchData>::iterator iter = m_touches.find(touchID);
    
    if(iter != m_touches.end())
    {
        TouchData* data = &(iter->second);
        
        return data->phase;
    }
    
    return M_TOUCH_NONE;
}

void Input::flush(void)
{
	// keys
	map<string, int>::iterator
		mit (m_keys.begin()),
		mend(m_keys.end());

	for(; mit!=mend; mit++)
	{
		if(mit->second == 1)
			mit->second = 2;
		else if(mit->second == 3)
			mit->second = 0;
	}
    
	// axis
	unsigned int a, aSize = m_axisToFlush.size();
	for(a=0; a<aSize; a++)
		(*m_axisToFlush[a]) = 0;
	
    // touches
    map<int, TouchData>::iterator
		t_it(m_touches.begin()),
		t_end(m_touches.end());
    
    for(; t_it!=t_end; t_it++)
    {
		TouchData* data = &(t_it->second);
        data->phase = M_TOUCH_NONE;
        data->touchPoint = MVector2(0.0f);
    }
}
