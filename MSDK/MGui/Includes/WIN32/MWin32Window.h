/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWin32Window.h
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


#ifndef _M_WIN32_WINDOW_H
#define _M_WIN32_WINDOW_H

#ifdef __CYGWIN__
#include <w32api.h>
#endif

#include <MCore.h>
#include <windows.h>
#include <list>


class MWindow
{
private:

	// windows contexts
	HDC m_hDC;
	HWND m_hWnd;
	HGLRC m_hRC;
	HINSTANCE m_hInstance;

	// infos
	bool m_focus;
	bool m_active;
	bool m_fullscreen;

	int	m_position[2];
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_colorBits;

	char m_title[256];
	char m_workingDirectory[256];

	// joysticks
	MJoystick m_joysticks[2];

	// events function pointer
	void (* m_pointerEvent)(MWinEvent * windowEvent);

public:

	MWindow(void);
	~MWindow(void);

	// instance
	static MWindow * getInstance(void)
	{
		static MWindow m_instance;
		return &m_instance;
	};

public:

	// create
	bool create(const char * title, unsigned int width, unsigned int height, int colorBits, bool fullscreen);

	// events
	inline void setPointerEvent(void (*pointerEvent)(MWinEvent * windowEvent)){ m_pointerEvent = pointerEvent; }
	void sendEvents(MWinEvent * events);
	bool onEvents(void);

	// cursor
	void setCursorPos(int x, int y);
	void hideCursor(void);
	void showCursor(void);

	// properties
	bool getFocus(void){ return m_focus; }
	inline bool isActive(void){ return m_active; }
	inline bool isFullscreen(void){ return m_fullscreen; }

	void setTitle(const char * title);
	void setFullscreen(bool fullscreen);
	void setFocus(bool focus){ m_focus = focus; }
	inline void setActive(bool active){ m_active = active; }

	// coords
	bool isMouseOverWindow(void);

	inline int getXPosition(void){ return m_position[0]; }
	inline int getYPosition(void){ return m_position[1]; }
	inline unsigned int getWidth(void){ return m_width; }
	inline unsigned int getHeight(void){ return m_height; }

	// swap buffer
	void swapBuffer(void);

	// system tick
	static unsigned long getSystemTick(void);

	// directories
	void setCurrentDirectory(const char * directory);
	void setWorkingDirectory(const char * directory);

	const char * getTempDirectory(void);
	const char * getCurrentDirectory(void);
	inline const char * getWorkingDirectory(void){ return m_workingDirectory; }

	// joysticks
	MJoystick * getJoystick1(void){ return &m_joysticks[0]; }
	MJoystick * getJoystick2(void){ return &m_joysticks[1]; }

	// execute
	void execute(const char * path, const char * args);

	// sleep
	void sleep(double time);
	
	// files parsing
    bool getOpenMultipleFiles(const char * title, const char * filter, string * repertory, list <string> * filesList);
    const char * getOpenDir(const char * title, const char * startPath = NULL);
	const char * getOpenFilename(const char * title, const char * filter, const char * startPath = NULL);
    const char * getSaveFilename(const char * title, const char * filter, const char * startPath = NULL);
};

#define MKEY_BACKSPACE	128
#define MKEY_TAB		129
#define MKEY_CLEAR		130
#define MKEY_RETURN		131
#define MKEY_PAUSE		132
#define MKEY_ESCAPE		133
#define MKEY_SPACE		32
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

#endif
