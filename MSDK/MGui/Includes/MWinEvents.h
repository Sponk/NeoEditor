/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MWinEvent.h
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


#ifndef _M_WIN_EVENTS_H
#define _M_WIN_EVENTS_H


#define MMOUSE_BUTTON_LEFT 0
#define MMOUSE_BUTTON_MIDDLE 1
#define MMOUSE_BUTTON_RIGHT 2

enum MWIN_EVENT_TYPE
{
	MWIN_EVENT_KEY_DOWN = 0,		// Key pressed
	MWIN_EVENT_KEY_UP,				// Key released
	MWIN_EVENT_CHAR,				// Char key pressed
	MWIN_EVENT_WINDOW_RESIZE,		// MWindow resized
	MWIN_EVENT_WINDOW_CLOSE,		// MWindow closed
	MWIN_EVENT_WINDOW_MOVE,			// MWindow moved
	MWIN_EVENT_MOUSE_MOVE,			// Mouse moved
	MWIN_EVENT_MOUSE_WHEEL_MOVE,	// Mouse wheel moved
	MWIN_EVENT_MOUSE_BUTTON_DOWN,	// Mouse button pressed
	MWIN_EVENT_MOUSE_BUTTON_UP,		// Mouse button released
	MWIN_EVENT_JOYSTICK1_UPDATE,	// Joystick 1
	MWIN_EVENT_JOYSTICK2_UPDATE,	// Joystick 2
};

struct MWinEvent
{
	MWIN_EVENT_TYPE type;
	int data[2];
};

#endif
