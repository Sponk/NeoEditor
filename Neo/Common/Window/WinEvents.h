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


#define MKEY_SPACE		32

#define MKEY_0			48
#define MKEY_1			49
#define MKEY_2			50
#define MKEY_3			51
#define MKEY_4			52
#define MKEY_5			53
#define MKEY_6			54
#define MKEY_7			55
#define MKEY_8			56
#define MKEY_9			57

#define MKEY_A			65
#define MKEY_B			66
#define MKEY_C			67
#define MKEY_D			68
#define MKEY_E			69
#define MKEY_F			70
#define MKEY_G			71
#define MKEY_H			72
#define MKEY_I			73
#define MKEY_J			74
#define MKEY_K			75
#define MKEY_L			76
#define MKEY_M			77
#define MKEY_N			78
#define MKEY_O			79
#define MKEY_P			80
#define MKEY_Q			81
#define MKEY_R			82
#define MKEY_S			83
#define MKEY_T			84
#define MKEY_U			85
#define MKEY_V			86
#define MKEY_W			87
#define MKEY_X			88
#define MKEY_Y			89
#define MKEY_Z			90

#define MKEY_BACKSPACE	128
#define MKEY_TAB		129
#define MKEY_CLEAR		130
#define MKEY_RETURN		131
#define MKEY_PAUSE		132
#define MKEY_ESCAPE		133
#define MKEY_DELETE		134
#define MKEY_KP0		135
#define MKEY_KP1		136
#define MKEY_KP2		137
#define MKEY_KP3		138
#define MKEY_KP4		139
#define MKEY_KP5		140
#define MKEY_KP6		141
#define MKEY_KP7		142
#define MKEY_KP8		143
#define MKEY_KP9		144
#define MKEY_KP_ENTER	145
#define MKEY_UP			146
#define MKEY_DOWN		147
#define MKEY_RIGHT		148
#define MKEY_LEFT		149
#define MKEY_INSERT		150
#define MKEY_HOME		151
#define MKEY_END		152
#define MKEY_PAGEUP		153
#define MKEY_PAGEDOWN	154
#define MKEY_F1			155
#define MKEY_F2			156
#define MKEY_F3			157
#define MKEY_F4			158
#define MKEY_F5			159
#define MKEY_F6			160
#define MKEY_F7			161
#define MKEY_F8			162
#define MKEY_F9			163
#define MKEY_F10		164
#define MKEY_F11		165
#define MKEY_F12		166
#define MKEY_NUMLOCK	167
#define MKEY_RSHIFT		168
#define MKEY_LSHIFT		169
#define MKEY_SHIFT		169 // deprecated
#define MKEY_RCONTROL	171
#define MKEY_LCONTROL	172
#define MKEY_CONTROL	172 // deprecated
#define MKEY_RALT		174
#define MKEY_LALT		175
#define MKEY_ADD		176
#define MKEY_SUB		177
#define MKEY_HELP		178
#define MKEY_PRINT		179
#define MKEY_MENU		180
#define MKEY_RSUPER		181
#define MKEY_LSUPER		182
#define MKEY_DUMMY		255

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
