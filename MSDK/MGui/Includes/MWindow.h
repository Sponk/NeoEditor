/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWindow.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2014 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) Anders Dahnielson <anders@dahnielson.com>
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


#if !defined(_M_WINDOW_H) && !defined(ANDROID)
#define _M_WINDOW_H

#include <MWinEvents.h>

#ifndef EMSCRIPTEN
	#include <SDL_joystick.h>
	#include <SDL_gamecontroller.h>
#endif

#include <vector>

class MWindow
{
private:
#ifndef EMSCRIPTEN
	// joysticks
	struct JoystickDevice_t
	{
		SDL_JoystickID id;
		SDL_Joystick * device;
	};

	std::vector<JoystickDevice_t *> m_joysticks;

	// game controllers
	struct GameControllerDevice_t
	{
		SDL_JoystickID id;
		SDL_GameController * device;
	};

	std::vector<GameControllerDevice_t *> m_controllers;
#endif
	int addGameController(int index);
	int removeGameController(int id);
	
	int addJoystick(int index);
	int removeJoystick(int id);

private:

	// infos
	bool m_focus;
	bool m_active;
	bool m_fullscreen;

	int	m_position[2];
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_colorBits;

	char m_workingDirectory[256];

	// events function pointer
	void (* m_pointerEvent)(MWinEvent * windowEvents);

public:

	MWindow(void);
	~MWindow(void);

	// instance
	static MWindow * getInstance(void)
	{
		static MWindow m_instance;
		return &m_instance;
	}

public:

	// create
	bool create(const char * title, unsigned int width, unsigned int height, int colorBits, bool fullscreen);
    void resize(unsigned int width, unsigned int height);

	// events
	inline void setPointerEvent(void (*pointerEvent)(MWinEvent * windowEvents)){ m_pointerEvent = pointerEvent; }
	void sendEvents(MWinEvent * event);
	bool onEvents(void);
    bool onWindowEvents(void);

	// cursor
	void setCursorPos(int x, int y);
	void hideCursor(void);
	void showCursor(void);

	// properties
	inline bool getFocus(void){ return m_focus; }
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

	// execute
	void execute(const char * path, const char * args);

	// sleep
	void sleep(double time);

    // messagebox
    void messagebox(const char* content, const char* title);
};

#else
#include <cctype>
#include <MAndroidWindow.h>
#endif
