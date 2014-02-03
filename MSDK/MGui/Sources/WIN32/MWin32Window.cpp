/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWin32Window.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) some code from GLFW <www.glfw.org>
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


#include <stdio.h>
#include <MCore.h>

#define _WIN32_WINNT 0x0500
#include <MWindow.h>

#if defined(__MINGW32__) || defined(__CYGWIN__)
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <zmouse.h>
#include <windows.h>
#include <mmsystem.h>
#include <Shlobj.h>

#include <GL/gl.h>
#include <GL/glu.h>


// windows contexts
static HDC m_hDC;
static HWND m_hWnd;
static HGLRC m_hRC;
static HINSTANCE m_hInstance;
static int cursorVis = 1;


MWindow::MWindow(void):
m_focus(true),
m_active(true),
m_fullscreen(false),
m_width(0),
m_height(0),
m_colorBits(0),
m_pointerEvent(NULL),
m_hDC(NULL),
m_hRC(NULL),
m_hWnd(NULL)
{
	m_position[0] = 0;
	m_position[1] = 0;
	strcpy(m_workingDirectory, getCurrentDirectory());
	strcpy(m_title, "");
}

MWindow::~MWindow(void)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);
	ReleaseDC(m_hWnd, m_hDC);
}

const char * MWindow::getTempDirectory(void)
{
	static char tempDirectory[256];
	TCHAR shortPath[MAX_PATH];
	
	GetTempPath(MAX_PATH, shortPath);
	
	LPCTSTR lpszShortPath = shortPath;
	DWORD buffSize = MAX_PATH;
	LPTSTR lpszLongPath = (LPTSTR)malloc(buffSize *sizeof(TCHAR));
	DWORD result = GetLongPathName(lpszShortPath, lpszLongPath, buffSize);
	
	if(result > MAX_PATH)
	{
		delete lpszLongPath;
		buffSize = result;
		lpszLongPath = (LPTSTR)malloc(buffSize*sizeof(TCHAR));
		if(! GetLongPathName(lpszShortPath, lpszLongPath, buffSize))
			return NULL;
	}
	
	strcpy(tempDirectory, lpszLongPath);
	return tempDirectory;
}

const char * MWindow::getCurrentDirectory(void)
{
	static char currentDirectory[256];
	getcwd(currentDirectory, 256);
	return currentDirectory;
}

void MWindow::setTitle(const char * title)
{
	SetWindowText(m_hWnd, title);
}

void MWindow::setCurrentDirectory(const char * directory)
{
	SetCurrentDirectory(directory);
}

void MWindow::setWorkingDirectory(const char * directory)
{
	strcpy(m_workingDirectory, directory);
}

void MWindow::sendEvents(MWinEvent * events)
{
	MKeyboard * keyboard = MKeyboard::getInstance();
	MMouse * mouse = MMouse::getInstance();
	
	switch(events->type)
	{
		case MWIN_EVENT_KEY_DOWN:
			keyboard->onKeyDown(events->data[0]);
			break;
			
		case MWIN_EVENT_KEY_UP:
			keyboard->onKeyUp(events->data[0]);
			break;
			
		case MWIN_EVENT_WINDOW_RESIZE:
			m_width = (unsigned int)events->data[0];
			m_height = (unsigned int)events->data[1];
			break;
			
		case MWIN_EVENT_WINDOW_MOVE:
			m_position[0] = events->data[0];
			m_position[1] = events->data[1];
			break;
			
		case MWIN_EVENT_MOUSE_BUTTON_DOWN:
			mouse->downButton(events->data[0]);
			break;
			
		case MWIN_EVENT_MOUSE_BUTTON_UP:
			mouse->upButton(events->data[0]);
			break;
			
		case MWIN_EVENT_MOUSE_WHEEL_MOVE:
			mouse->setWheelDirection(events->data[0]);
			break;
	}
	
	if(m_pointerEvent)
		m_pointerEvent(events);
}

void MWindow::setCursorPos(int x, int y)
{
	SetCursorPos(x, y);
}

void MWindow::hideCursor(void)
{
	if(cursorVis == 1)
	{
		ShowCursor(0);
		cursorVis = 0;
	}
}

void MWindow::showCursor(void)
{
	if(cursorVis == 0)
	{
		ShowCursor(1);
		cursorVis = 1;
	}
}

bool MWindow::isMouseOverWindow(void)
{
	POINT p;
	
	GetCursorPos(&p);
	p.x -= getXPosition();
	p.y -= getYPosition();
	
	if(p.x >= 0 && p.y >= 0 && p.x < (int)getWidth() && p.y < (int)getHeight())
		return true;
	
	return false;
}

unsigned long MWindow::getSystemTick(void)
{
	return GetTickCount();
}

void updateJoystick(MJoystick * joystick, JOYCAPS * caps, JOYINFOEX * joyinfo)
{
	int acces = 1;
	for(int i=0; i<8; i++)
	{
		bool pressed = joystick->isKeyPressed(i);
		if(joyinfo->dwButtons & acces)
		{
			if(! pressed)
				joystick->downKey(i);
		}
		else
		{
			if(pressed)
				joystick->upKey(i);
		}
		
		acces += acces;
	}
	
	float dx = (float)(caps->wXmax - caps->wXmin);
	float dy = (float)(caps->wYmax - caps->wYmin);
	float dz = (float)(caps->wZmax - caps->wZmin);
	float dr = (float)(caps->wRmax - caps->wRmin);
	float du = (float)(caps->wUmax - caps->wUmin);
	float dv = (float)(caps->wVmax - caps->wVmin);
	
	float x = ((float)joyinfo->dwXpos / dx)*2.0f - 1.0f;
	float y = ((float)joyinfo->dwYpos / dx)*2.0f - 1.0f;
	float z = ((float)joyinfo->dwZpos / dx)*2.0f - 1.0f;
	float r = ((float)joyinfo->dwRpos / dx)*2.0f - 1.0f;
	float u = ((float)joyinfo->dwUpos / dx)*2.0f - 1.0f;
	float v = ((float)joyinfo->dwVpos / dx)*2.0f - 1.0f;
	
	joystick->setX(x);
	joystick->setY(y);
	joystick->setZ(z);
	joystick->setR(r);
	joystick->setU(u);
	joystick->setV(v);
}

bool MWindow::onEvents(void)
{
	JOYCAPS caps;
	MWindow * window = MWindow::getInstance();
	
	// joystick 1
	if(joyGetDevCaps(JOYSTICKID1, &caps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
	{
		JOYINFOEX joyinfo;
		MJoystick * joystick = window->getJoystick1();
		
		joyinfo.dwSize = sizeof(JOYINFOEX);
		memset(&(joyinfo.dwFlags), 0, sizeof(JOYINFOEX) - sizeof(DWORD));
		joyinfo.dwFlags = JOY_RETURNALL;
		
		joyGetPosEx(JOYSTICKID1, &joyinfo);
		updateJoystick(joystick, &caps, &joyinfo);
		
		MWinEvent events;
		events.type = MWIN_EVENT_JOYSTICK1_UPDATE;
		window->sendEvents(&events);
		
		joystick->flush();
	}
	
	// joystick 2
	if(joyGetDevCaps(JOYSTICKID2, &caps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
	{
		JOYINFOEX joyinfo;
		MJoystick * joystick = window->getJoystick2();
		
		joyinfo.dwSize = sizeof(JOYINFOEX);
		memset(&(joyinfo.dwFlags), 0, sizeof(JOYINFOEX) - sizeof(DWORD));
		joyinfo.dwFlags = JOY_RETURNALL;
		
		joyGetPosEx(JOYSTICKID2, &joyinfo);
		updateJoystick(joystick, &caps, &joyinfo);
		
		MWinEvent events;
		events.type = MWIN_EVENT_JOYSTICK2_UPDATE;
		window->sendEvents(&events);
		
		joystick->flush();
	}
	
	// mouse move event
	{
		POINT p;
		MWinEvent events;
		MMouse * mouse = MMouse::getInstance();
		
		GetCursorPos(&p);
		p.x -= window->getXPosition();
		p.y -= window->getYPosition();
		
		int dx = p.x - mouse->getXPosition();
		int dy = p.y - mouse->getYPosition();
		if((dx != 0) || (dy != 0))
		{
			mouse->setPosition(p.x, p.y);
			
			events.type = MWIN_EVENT_MOUSE_MOVE;
			events.data[0] = p.x;
			events.data[1] = p.y;
			window->sendEvents(&events);
		}
	}
	
	MSG msg;
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return false;
	}
	
	return true;
}

void MWindow::swapBuffer(void)
{
	SwapBuffers(m_hDC);
}

int translateKey( WPARAM wParam, LPARAM lParam )
{
    MSG next_msg;
    DWORD msg_time;
    DWORD scan_code;
	
    // Check for numeric keypad keys
    // Note: This way we always force "NumLock = ON", which at least
    // enables GLFW users to detect numeric keypad keys
    int hiFlags = HIWORD( lParam );
	
    if ( !( hiFlags & 0x100 ) )
    {
        switch( MapVirtualKey( hiFlags & 0xFF, 1 ) )
        {
            case VK_INSERT:   return MKEY_KP0;
            case VK_END:      return MKEY_KP1;
            case VK_DOWN:     return MKEY_KP2;
            case VK_NEXT:     return MKEY_KP3;
            case VK_LEFT:     return MKEY_KP4;
            case VK_CLEAR:    return MKEY_KP5;
            case VK_RIGHT:    return MKEY_KP6;
            case VK_HOME:     return MKEY_KP7;
            case VK_UP:       return MKEY_KP8;
            case VK_PRIOR:    return MKEY_KP9;
				//case VK_DIVIDE:   return MKEY_KP_DIVIDE;
				//case VK_MULTIPLY: return MKEY_KP_MULTIPLY;
            case VK_SUBTRACT: return MKEY_SUB;
            case VK_ADD:      return MKEY_ADD;
            case VK_DELETE:   return MKEY_DELETE;
        }
    }
	
    // Check which key was pressed or released
    switch( wParam )
    {
			// The SHIFT keys require special handling
        case VK_SHIFT:
        {
            // Compare scan code for this key with that of VK_RSHIFT in
            // order to determine which shift key was pressed (left or
            // right)
            scan_code = MapVirtualKey( VK_RSHIFT, 0 );
            if( ((lParam & 0x01ff0000) >> 16) == scan_code )
            {
                return MKEY_RSHIFT;
            }
			
            return MKEY_LSHIFT;
        }
			
			// The CTRL keys require special handling
        case VK_CONTROL:
        {
            // Is this an extended key (i.e. right key)?
            if( lParam & 0x01000000 )
            {
                return MKEY_RCONTROL;
            }
			
            // Here is a trick: "Alt Gr" sends LCTRL, then RALT. We only
            // want the RALT message, so we try to see if the next message
            // is a RALT message. In that case, this is a false LCTRL!
            msg_time = GetMessageTime();
            if( PeekMessage( &next_msg, NULL, 0, 0, PM_NOREMOVE ) )
            {
                if( next_msg.message == WM_KEYDOWN ||
				   next_msg.message == WM_SYSKEYDOWN )
                {
                    if( next_msg.wParam == VK_MENU &&
					   (next_msg.lParam & 0x01000000) &&
					   next_msg.time == msg_time )
                    {
                        // Next message is a RALT down message, which
                        // means that this is NOT a proper LCTRL message!
                        return -1;
                    }
                }
            }
			
            return MKEY_LCONTROL;
        }
			
			// The ALT keys require special handling
        case VK_MENU:
        {
            // Is this an extended key (i.e. right key)?
            if( lParam & 0x01000000 )
            {
                return MKEY_RALT;
            }
			
            return MKEY_LALT;
        }
			
			// The ENTER keys require special handling
        case VK_RETURN:
        {
            // Is this an extended key (i.e. right key)?
            if( lParam & 0x01000000 )
            {
                return MKEY_KP_ENTER;
            }
			
            return MKEY_RETURN;
        }
			
			// Special keys (non character keys)
        case VK_ESCAPE:        return MKEY_ESCAPE;
        case VK_TAB:           return MKEY_TAB;
        case VK_BACK:          return MKEY_BACKSPACE;
        case VK_HOME:          return MKEY_HOME;
        case VK_END:           return MKEY_END;
        case VK_PRIOR:         return MKEY_PAGEUP;
        case VK_NEXT:          return MKEY_PAGEDOWN;
        case VK_INSERT:        return MKEY_INSERT;
        case VK_DELETE:        return MKEY_DELETE;
        case VK_LEFT:          return MKEY_LEFT;
        case VK_UP:            return MKEY_UP;
        case VK_RIGHT:         return MKEY_RIGHT;
        case VK_DOWN:          return MKEY_DOWN;
        case VK_F1:            return MKEY_F1;
        case VK_F2:            return MKEY_F2;
        case VK_F3:            return MKEY_F3;
        case VK_F4:            return MKEY_F4;
        case VK_F5:            return MKEY_F5;
        case VK_F6:            return MKEY_F6;
        case VK_F7:            return MKEY_F7;
        case VK_F8:            return MKEY_F8;
        case VK_F9:            return MKEY_F9;
        case VK_F10:           return MKEY_F10;
        case VK_F11:           return MKEY_F11;
        case VK_F12:           return MKEY_F12;
			/*case VK_F13:           return MKEY_F13;
			 case VK_F14:           return MKEY_F14;
			 case VK_F15:           return MKEY_F15;
			 case VK_F16:           return MKEY_F16;
			 case VK_F17:           return MKEY_F17;
			 case VK_F18:           return MKEY_F18;
			 case VK_F19:           return MKEY_F19;
			 case VK_F20:           return MKEY_F20;
			 case VK_F21:           return MKEY_F21;
			 case VK_F22:           return MKEY_F22;
			 case VK_F23:           return MKEY_F23;
			 case VK_F24:           return MKEY_F24;*/
        case VK_SPACE:         return MKEY_SPACE;
			
			// Numeric keypad
        case VK_NUMPAD0:       return MKEY_KP0;
        case VK_NUMPAD1:       return MKEY_KP1;
        case VK_NUMPAD2:       return MKEY_KP2;
        case VK_NUMPAD3:       return MKEY_KP3;
        case VK_NUMPAD4:       return MKEY_KP4;
        case VK_NUMPAD5:       return MKEY_KP5;
        case VK_NUMPAD6:       return MKEY_KP6;
        case VK_NUMPAD7:       return MKEY_KP7;
        case VK_NUMPAD8:       return MKEY_KP8;
        case VK_NUMPAD9:       return MKEY_KP9;
			// case VK_DIVIDE:        return MKEY_KP_DIVIDE;
			//case VK_MULTIPLY:      return MKEY_KP_MULTIPLY;
        case VK_SUBTRACT:      return MKEY_SUB;
        case VK_ADD:           return MKEY_ADD;
			//case VK_DECIMAL:       return MKEY_KP_DECIMAL;
        case VK_NUMLOCK:       return MKEY_NUMLOCK;
			
			// case VK_CAPITAL:       return MKEY_CAPS_LOCK;
			//case VK_SCROLL:        return MKEY_SCROLL_LOCK;
        case VK_PAUSE:         return MKEY_PAUSE;
			
			//case VK_LWIN:          return MKEY_LSUPER;
			//case VK_RWIN:          return MKEY_RSUPER;
        case VK_APPS:          return MKEY_MENU;
			
			// The rest (should be printable keys)
        default:
        {
            // Convert to printable character (ISO-8859-1 or Unicode)
            wParam = MapVirtualKey( (UINT) wParam, 2 ) & 0x0000FFFF;
			
            // Make sure that the character is uppercase
            wParam = (WPARAM) CharUpperW( (LPWSTR) wParam );
			
            // Valid ISO-8859-1 character?
            if( (wParam >=  32 && wParam <= 126) ||
			   (wParam >= 160 && wParam <= 255) )
            {
                return (int) wParam;
            }
			
            return -1;
        }
    }
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MMouse * mouse = MMouse::getInstance();
	MWindow * window = MWindow::getInstance();
	MWinEvent events;
	
	if(window->isActive())
	{
        switch(uMsg)
        {
			case WM_CREATE:
            {
                joySetCapture(hwnd, JOYSTICKID1, 0, FALSE);
                joySetCapture(hwnd, JOYSTICKID2, 0, FALSE);
            }
				break;
				
			case WM_KILLFOCUS:
				window->setFocus(false);
				break;
				
			case WM_SETFOCUS:
				window->setFocus(true);
				break;
				
			case WM_CLOSE:
				events.type = MWIN_EVENT_WINDOW_CLOSE;
				window->sendEvents(&events);
				return 0;
				
			case WM_DESTROY:
            {
                joyReleaseCapture(JOYSTICKID1);
                joyReleaseCapture(JOYSTICKID2);
            }
				break;
				
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
				events.type = MWIN_EVENT_KEY_DOWN;
				events.data[0] = translateKey(wParam, lParam);//(int)wParam;
				if(events.data[0] >= 0)
					window->sendEvents(&events);
				break;
				
			case WM_SYSKEYUP:
			case WM_KEYUP:
				events.type = MWIN_EVENT_KEY_UP;
				events.data[0] = translateKey(wParam, lParam);//(int)wParam;
				if(events.data[0] >= 0)
					window->sendEvents(&events);
				break;
				
			case WM_SIZE:
				events.type = MWIN_EVENT_WINDOW_RESIZE;
				events.data[0] = (int)LOWORD(lParam);
				events.data[1] = (int)HIWORD(lParam);
				window->sendEvents(&events);
				break;
				
			case WM_MOVE:
				events.type = MWIN_EVENT_WINDOW_MOVE;
				events.data[0] = (int)(short)LOWORD(lParam);
				events.data[1] = (int)(short)HIWORD(lParam);
				window->sendEvents(&events);
				break;
				
			case WM_LBUTTONDOWN:
				SetCapture(hwnd);
				events.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
				events.data[0] = (int)MMOUSE_BUTTON_LEFT;
				window->sendEvents(&events);
				break;
				
			case WM_LBUTTONUP:
				ReleaseCapture();
				events.type = MWIN_EVENT_MOUSE_BUTTON_UP;
				events.data[0] = (int)MMOUSE_BUTTON_LEFT;
				window->sendEvents(&events);
				break;
				
			case WM_CHAR:
				if(wParam == 8 || wParam == 32 || wParam == 13 || wParam == 9) // not return, space, delete etc..
					break;
				
				events.type = MWIN_EVENT_CHAR;
				events.data[0] = (int)(char)wParam;
				window->sendEvents(&events);
				break;
				
			case WM_MBUTTONDOWN:
				SetCapture(hwnd);
				events.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
				events.data[0] = (int)MMOUSE_BUTTON_MIDDLE;
				window->sendEvents(&events);
				break;
				
			case WM_MBUTTONUP:
				ReleaseCapture();
				events.type = MWIN_EVENT_MOUSE_BUTTON_UP;
				events.data[0] = (int)MMOUSE_BUTTON_MIDDLE;
				window->sendEvents(&events);
				break;
				
			case WM_RBUTTONDOWN:
				SetCapture(hwnd);
				events.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
				events.data[0] = (int)MMOUSE_BUTTON_RIGHT;
				window->sendEvents(&events);
				break;
				
			case WM_RBUTTONUP:
				ReleaseCapture();
				events.type = MWIN_EVENT_MOUSE_BUTTON_UP;
				events.data[0] = (int)MMOUSE_BUTTON_RIGHT;
				window->sendEvents(&events);
				break;
				
			case WM_MOUSEWHEEL:
				events.type = MWIN_EVENT_MOUSE_WHEEL_MOVE;
				events.data[0] = CLAMP((int)((short)HIWORD(wParam) / (int)WHEEL_DELTA), -1, 1);
				window->sendEvents(&events);
				break;
        }
    }
	else
	{
		switch(uMsg)
        {
			case WM_NCPAINT:
				window->swapBuffer();
				break;
        }
	}
	
	// Pass All unhandled Messages To DefWindowProc
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool MWindow::create(const char * title, unsigned int width, unsigned int height, int colorBits, bool fullscreen)
{
	GLuint PixelFormat;
	WNDCLASSA wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	
	RECT windowRect;
	windowRect.left = 0;
	windowRect.right = (long)width;
	windowRect.top = 0;
	windowRect.bottom = (long)height;
	
	m_hInstance                     = GetModuleHandle(NULL);                                // Grab An Instance For Our MWindow
	wc.style                        = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;   // Redraw On Size, And Own DC For MWindow.
	wc.lpfnWndProc          = (WNDPROC) WndProc;                                    // WndProc Handles Messages
	wc.cbClsExtra           = 0;                                                                    // No Extra MWindow Data
	wc.cbWndExtra           = 0;                                                                    // No Extra MWindow Data
	wc.hInstance            = m_hInstance;                                                  // Set The Instance
	wc.hIcon                        = LoadIcon(m_hInstance, MAKEINTRESOURCE(102));//IDI_WINLOGO);                   // Load The Default Icon
	wc.hCursor                      = LoadCursor(NULL, IDC_ARROW);                  // Load The Arrow Pointer
	wc.hbrBackground        = NULL;                                                                 // No Background Required For GL
	wc.lpszMenuName         = NULL;                                                                 // We Don't Want A Menu
	wc.lpszClassName        = "OpenGL";                                                             // Set The Class Name
	
	if(! RegisterClassA(&wc))
	{
		printf("MWindow : unable to create window class\n");
		return false;
	}
	
	// configure fullscreen
	if(fullscreen)
	{
		DEVMODEA dmScreenSettings;                                                              // Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));   // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);               // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;                                   // Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;                                 // Selected Screen Height
		dmScreenSettings.dmBitsPerPel = colorBits;                              // Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		
		if(ChangeDisplaySettingsA(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			printf("MWindow : unable to configure fullscreen\n");
			return false;
		}
	}
	
	// window style
	if(fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}
	
	// adjust window rect
	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);
	
	// create window
	if(! (m_hWnd = CreateWindowExA(
								   dwExStyle,                                                   // Extended Style For The MWindow
								   "OpenGL",                                                    // Class Name
								   title,                                                               // MWindow Title
								   dwStyle,                                                     // MWindow Style
								   0, 0,                                                                // MWindow Position
								   windowRect.right - windowRect.left,
								   windowRect.bottom - windowRect.top,  // Selected Width And Height
								   NULL,                                                                // Parent MWindow
								   NULL,                                                                // Menu
								   m_hInstance,                                         // Instance
								   NULL)))
	{
		printf("MWindow : unable to create window\n");
		return false;
	}
	
	// pixel format description
	static  PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),                          // Size Of This Pixel Format Descriptor
		1,                                                                                      // Version Number
		PFD_DRAW_TO_WINDOW |                                            // Format Must Support MWindow
		PFD_SUPPORT_OPENGL |                                            // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                                                       // Must Support Double Buffering
		PFD_TYPE_RGBA,                                                          // Request An RGBA Format
		colorBits,                                                                      // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                                                       // Color Bits Ignored
		0,                                                                                      // No Alpha Buffer
		0,                                                                                      // Shift Bit Ignored
		0,                                                                                      // No Accumulation Buffer
		0, 0, 0, 0,                                                                     // Accumulation Bits Ignored
		16,                                                                                     // 16Bit Z-Buffer (Depth Buffer)
		8,                                                                                      // Stencil Buffer
		0,                                                                                      // No Auxiliary Buffer
		PFD_MAIN_PLANE,                                                         // Main Drawing Layer
		0,                                                                                      // Reserved
		0, 0, 0                                                                         // Layer Masks Ignored
	};
	
	if(! (m_hDC = GetDC(m_hWnd)))
	{
		printf("MWindow : unable to create context device\n");
		return false;
	}
	
	if(! (PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
	{
		printf("MWindow : unable to find pixel format\n");
		return false;
	}
	
	if(! SetPixelFormat(m_hDC, PixelFormat, &pfd))
	{
		printf("MWindow : unable to set pixel format\n");
		return false;
	}
	
	if(! (m_hRC = wglCreateContext(m_hDC)))
	{
		printf("MWindow : unable to create opengl context\n");
		return false;
	}
	
	if(! wglMakeCurrent(m_hDC,m_hRC))
	{
		printf("MWindow : unable to set opengl current context\n");
		return false;
	}
	
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	
	strcpy(m_title, title);
	
	//SystemParametersInfo(SPI_GETWORKAREA,0,&DesktopArea,0);
	
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	
	m_width = rect.right - rect.left;
	m_height = rect.bottom - rect.top;
	m_colorBits = colorBits;
	m_fullscreen = fullscreen;
	
#ifdef M_HIDE_CONSOLE
	FreeConsole();
#endif
	
	return true;
}

void strrep(char *str, char o, char n)
{
    char *pos = strchr(str, o);
    while (pos != NULL)  {
        *pos = n;
        pos = strchr(pos + 1, o);
    }
}

int CALLBACK browseCallbackProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case BFFM_INITIALIZED:
            SendMessage(hwnd, BFFM_SETSELECTION, FALSE, lParam);
			break;
	}
	return 0;
}

void MWindow::execute(const char * path, const char * args)
{
	ShellExecute(NULL, "open", path, args, NULL, SW_SHOWNORMAL);
}

void MWindow::sleep(double time)
{
	DWORD t;
	
    if( time == 0.0 )
    {
		t = 0;
    }
    else if( time < 0.001 )
    {
        t = 1;
    }
    else if( time > 2147483647.0 )
    {
        t = 2147483647;
    }
    else
    {
        t = (DWORD)(time*1000.0 + 0.5);
    }
    Sleep( t );
}

bool MWindow::getOpenMultipleFiles(const char * title, const char * filter, string * repertory, list <string> * filesList)
{
	const char * currentDir = getCurrentDirectory();
	setActive(false);
	
	static char filename[65536];
	OPENFILENAME fn;
	
	memset(&fn, 0, sizeof(fn));
	strcpy(filename, "");
	
	fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	fn.lStructSize  = sizeof(fn);
	fn.lpstrFilter  = filter;
	fn.lpstrFile    = filename;
	fn.nMaxFile             = 65536*4;
	fn.lpstrTitle   = title;
	fn.hwndOwner    = m_hWnd;
	filename[0]             = NULL;
	
	if(GetOpenFileName(&fn))
	{
		setCurrentDirectory(currentDir);
		
		if(fn.nFileOffset < lstrlen(filename))
		{
			char rep[256];
			getRepertory(rep, filename);
			getLocalFilename(filename, rep, filename);
			(*repertory) = rep;
			filesList->push_back(filename);
		}
		else
		{
			char filePath[256];
			strcpy(filePath, filename);
			filePath[fn.nFileOffset] = 0;
			(*repertory) = filePath;
			
			while(filename[fn.nFileOffset] != 0)
			{
				char Message[256];
				strcpy(Message, filename+fn.nFileOffset);
				filesList->push_back(Message);
				fn.nFileOffset += (strlen(filename+fn.nFileOffset) + 1);
			}
		}
		setActive(true);
		return true;
	}
	
	setActive(true);
	return false;
}

const char * MWindow::getOpenDir(const char * title, const char * startPath)
{
	setActive(false);
	
	static char filename[256];
	
	static BROWSEINFO bi;
	ITEMIDLIST * pItem;
	
	LPSTR szBuffer;
	
	LPMALLOC ppMalloc;
	SHGetMalloc(&ppMalloc);
	
	szBuffer = (LPSTR)ppMalloc->Alloc(MAX_PATH);
	
	bi.hwndOwner = m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szBuffer;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	bi.lpfn = (BFFCALLBACK)browseCallbackProc;
	
	pItem = SHBrowseForFolder(&bi);
	
	if(pItem)
	{
		SHGetPathFromIDList(pItem, szBuffer);
		bi.lParam = (LPARAM)pItem;
		
		strcpy(filename, szBuffer);
		setActive(true);
		return filename;
		
		ppMalloc->Free(pItem);
		ppMalloc->Free(szBuffer);
	}
	else
	{
		ppMalloc->Free(pItem);
		ppMalloc->Free(szBuffer);
	}
	
	setActive(true);
	return NULL;
}

const char * MWindow::getOpenFilename(const char * title, const char * filter, const char * startPath)
{
	char winStartPath[256] = "";
	if(startPath)
	{
		strcpy(winStartPath, startPath);
		strrep(winStartPath, '/', '\\');
	}
	
	const char * currentDir = getCurrentDirectory();
	setActive(false);
	
	static char filename[256];
	OPENFILENAME fn;
	
	memset(&fn, 0, sizeof(fn));
	strcpy(filename, "");
	
	fn.lpstrInitialDir = winStartPath;
	fn.lStructSize  = sizeof(fn);
	fn.lpstrFilter  = filter;
	fn.lpstrFile    = filename;
	fn.nMaxFile             = 256*4;
	fn.lpstrTitle   = title;
	filename[0]             = NULL;
	fn.hwndOwner    = m_hWnd;
	
	if(GetOpenFileName(&fn))
	{
		setCurrentDirectory(currentDir);
		setActive(true);
		return filename;
	}
	
	setActive(true);
	return NULL;
}

const char * MWindow::getSaveFilename(const char * title, const char * filter, const char * startPath)
{
	char winStartPath[256] = "";
	if(startPath)
	{
		strcpy(winStartPath, startPath);
		strrep(winStartPath, '/', '\\');
	}
	
	const char * currentDir = getCurrentDirectory();
	setActive(false);
	
	static char filename[256];
	OPENFILENAME fn;
	
	memset(&fn, 0, sizeof(fn));
	strcpy(filename, "");
	
	fn.lpstrInitialDir = winStartPath;
	fn.lStructSize  = sizeof(fn);
	fn.lpstrFilter  = filter;
	fn.lpstrFile    = filename;
	fn.nMaxFile             = 256*4;
	fn.Flags                = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	fn.lpstrTitle   = title;
	filename[0]             = NULL;
	fn.hwndOwner    = m_hWnd;
	
	if(GetSaveFileName(&fn))
	{
		setCurrentDirectory(currentDir);
		setActive(true);
		return filename;
	}
	
	setActive(true);
	return NULL;
}