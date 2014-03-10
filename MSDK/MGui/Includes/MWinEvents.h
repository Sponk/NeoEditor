/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MWinEvent.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014 Anders Dahnielson <anders@dahnielson.com>
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


#ifndef _M_WIN_EVENTS_H
#define _M_WIN_EVENTS_H


enum MMOUSE_BUTTON
{
	MMOUSE_BUTTON_INVALID = -1,
	MMOUSE_BUTTON_LEFT,
	MMOUSE_BUTTON_MIDDLE,
	MMOUSE_BUTTON_RIGHT,
	MMOUSE_BUTTON_X1,
	MMOUSE_BUTTON_X2,
	MMOUSE_BUTTON_MAX
};

enum MCONTROLLER_AXIS
{
	 MCONTROLLER_AXIS_INVALID = -1,
	 MCONTROLLER_AXIS_LEFTX,
	 MCONTROLLER_AXIS_LEFTY,
	 MCONTROLLER_AXIS_RIGHTX,
	 MCONTROLLER_AXIS_RIGHTY,
	 MCONTROLLER_AXIS_TRIGGERLEFT,
	 MCONTROLLER_AXIS_TRIGGERRIGHT,
	 MCONTROLLER_AXIS_MAX
};

enum MCONTROLLER_BUTTON
{
	MCONTROLLER_BUTTON_INVALID = -1,
	MCONTROLLER_BUTTON_A,
	MCONTROLLER_BUTTON_B,
	MCONTROLLER_BUTTON_X,
	MCONTROLLER_BUTTON_Y,
	MCONTROLLER_BUTTON_BACK,
	MCONTROLLER_BUTTON_GUIDE,
	MCONTROLLER_BUTTON_START,
	MCONTROLLER_BUTTON_LEFTSTICK,
	MCONTROLLER_BUTTON_RIGHTSTICK,
	MCONTROLLER_BUTTON_LEFTSHOULDER,
	MCONTROLLER_BUTTON_RIGHTSHOULDER,
	MCONTROLLER_BUTTON_DPADUP,
	MCONTROLLER_BUTTON_DPADDOWN,
	MCONTROLLER_BUTTON_DPADLEFT,
	MCONTROLLER_BUTTON_DPADRIGHT,
	MCONTROLLER_BUTTON_MAX
};

enum MWIN_EVENT_TYPE
{
	MWIN_EVENT_INVALID = -1,
	MWIN_EVENT_KEY_DOWN,			   // Key pressed
	MWIN_EVENT_KEY_UP,				   // Key released
	MWIN_EVENT_CHAR,				   // Char key pressed
	MWIN_EVENT_WINDOW_RESIZE,		   // MWindow resized
	MWIN_EVENT_WINDOW_CLOSE,		   // MWindow closed
	MWIN_EVENT_WINDOW_MOVE,			   // MWindow moved
	MWIN_EVENT_MOUSE_MOVE,			   // Mouse moved
	MWIN_EVENT_MOUSE_WHEEL_MOVE,	   // Mouse wheel moved
	MWIN_EVENT_MOUSE_BUTTON_DOWN,	   // Mouse button pressed
	MWIN_EVENT_MOUSE_BUTTON_UP,		   // Mouse button released
	MWIN_EVENT_JOYSTICK_MOVE,          // Joystick moved
	MWIN_EVENT_JOYSTICK_BALL_MOVE,     // Joystick ball moved
	MWIN_EVENT_JOYSTICK_HAT_MOVE,      // Joystick hat moved
	MWIN_EVENT_JOYSTICK_BUTTON_DOWN,   // Joystick button pressed
	MWIN_EVENT_JOYSTICK_BUTTON_UP,     // Joystick button released
	MWIN_EVENT_JOYSTICK_ADDED,         // Joystick added
	MWIN_EVENT_JOYSTICK_REMOVED,       // Joystick removed
	MWIN_EVENT_CONTROLLER_MOVE,        // Controller moved
	MWIN_EVENT_CONTROLLER_BUTTON_DOWN, // Controller button pressed
	MWIN_EVENT_CONTROLLER_BUTTON_UP,   // Controller button released
	MWIN_EVENT_CONTROLLER_ADDED,       // Controller added
	MWIN_EVENT_CONTROLLER_REMOVED,     // Controller removed
	MWIN_EVENT_CONTROLLER_REMAPPED,    // Controller remapped
	MWIN_EVENT_FINGER_DOWN,            // Finger down
	MWIN_EVENT_FINGER_UP,              // Finger up
	MWIN_EVENT_FINGER_MOVE,            // Finger moved
	MWIN_EVENT_MAX
};

struct MWinEvent
{
	MWIN_EVENT_TYPE type;
	int data[4];
};

#endif
