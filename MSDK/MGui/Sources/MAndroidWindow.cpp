/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MSDLWindow.cpp
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

#ifdef ANDROID
#include <MMouse.h>
#include <MKeyboard.h>
#include <MAndroidWindow.h>

#include <SDL.h>

#ifndef USE_GLES
#include <SDL_opengl.h>
#else
#ifdef __APPLE__
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#ifdef __ANDROID__
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#endif

#include "../../MEngine/Includes/MEngine.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

static SDL_Window * g_window;
static SDL_GLContext g_context;

static int translateKey(SDL_Keycode key)
{
	switch (key)
	{
		case SDLK_0:            return MKEY_0;
		case SDLK_1:            return MKEY_1;
		case SDLK_2:            return MKEY_2;
		case SDLK_3:            return MKEY_3;
		case SDLK_4:            return MKEY_4;
		case SDLK_5:            return MKEY_5;
		case SDLK_6:            return MKEY_6;
		case SDLK_7:            return MKEY_7;
		case SDLK_8:            return MKEY_8;
		case SDLK_9:            return MKEY_9;

		case SDLK_a:            return MKEY_A;
		case SDLK_b:            return MKEY_B;
		case SDLK_c:            return MKEY_C;
		case SDLK_d:            return MKEY_D;
		case SDLK_e:            return MKEY_E;
		case SDLK_f:            return MKEY_F;
		case SDLK_g:            return MKEY_G;
		case SDLK_h:            return MKEY_H;
		case SDLK_i:            return MKEY_I;
		case SDLK_j:            return MKEY_J;
		case SDLK_k:            return MKEY_K;
		case SDLK_l:            return MKEY_L;
		case SDLK_m:            return MKEY_M;
		case SDLK_n:            return MKEY_N;
		case SDLK_o:            return MKEY_O;
		case SDLK_p:            return MKEY_P;
		case SDLK_q:            return MKEY_Q;
		case SDLK_r:            return MKEY_R;
		case SDLK_s:            return MKEY_S;
		case SDLK_t:            return MKEY_T;
		case SDLK_u:            return MKEY_U;
		case SDLK_v:            return MKEY_V;
		case SDLK_w:            return MKEY_W;
		case SDLK_x:            return MKEY_X;
		case SDLK_y:            return MKEY_Y;
		case SDLK_z:            return MKEY_Z;

		case SDLK_KP_0:         return MKEY_KP0;
		case SDLK_KP_1:         return MKEY_KP1;
		case SDLK_KP_2:         return MKEY_KP2;
		case SDLK_KP_3:         return MKEY_KP3;
		case SDLK_KP_4:         return MKEY_KP4;
		case SDLK_KP_5:         return MKEY_KP5;
		case SDLK_KP_6:         return MKEY_KP6;
		case SDLK_KP_7:         return MKEY_KP7;
		case SDLK_KP_8:         return MKEY_KP8;
		case SDLK_KP_9:         return MKEY_KP9;
		//case SDLK_KP_MINUS:     return MKEY_KP_MINUS;
		//case SDLK_KP_PLUS:      return MKEY_KP_PLUS;
		//case SDLK_KP_DIVIDE:    return MKEY_KP_DIVIDE;
		//case SDLK_KP_MULTIPLY:  return MKEY_KP_MULTIPLY;
		case SDLK_KP_ENTER:     return MKEY_KP_ENTER;

		case SDLK_SPACE:        return MKEY_SPACE;
		case SDLK_ESCAPE:       return MKEY_ESCAPE;
		case SDLK_TAB:          return MKEY_TAB;
		case SDLK_LSHIFT:       return MKEY_LSHIFT;
		case SDLK_RSHIFT:       return MKEY_RSHIFT;
		case SDLK_LCTRL:        return MKEY_LCONTROL;
		case SDLK_RCTRL:        return MKEY_RCONTROL;
		case SDLK_LALT:         return MKEY_LALT;
		case SDLK_MODE:
		case SDLK_RALT:         return MKEY_RALT;
		case SDLK_LGUI:         return MKEY_LSUPER;
		case SDLK_RGUI:         return MKEY_RSUPER;
		case SDLK_MENU:         return MKEY_MENU;
		case SDLK_NUMLOCKCLEAR: return MKEY_NUMLOCK;
		//case SDLK_CAPSLOCK:     return MKEY_CAPS_LOCK;
		//case SDLK_SCROLLLOCK:   return MKEY_SCROLL;
		case SDLK_PAUSE:        return MKEY_PAUSE;
		case SDLK_DELETE:       return MKEY_DELETE;
		case SDLK_BACKSPACE:    return MKEY_BACKSPACE;
		case SDLK_RETURN:       return MKEY_RETURN;
		case SDLK_HOME:         return MKEY_HOME;
		case SDLK_END:          return MKEY_END;
		case SDLK_PAGEUP:       return MKEY_PAGEUP;
		case SDLK_PAGEDOWN:     return MKEY_PAGEDOWN;
		case SDLK_INSERT:       return MKEY_INSERT;
		case SDLK_LEFT:         return MKEY_LEFT;
		case SDLK_RIGHT:        return MKEY_RIGHT;
		case SDLK_DOWN:         return MKEY_DOWN;
		case SDLK_UP:           return MKEY_UP;
		case SDLK_F1:           return MKEY_F1;
		case SDLK_F2:           return MKEY_F2;
		case SDLK_F3:           return MKEY_F3;
		case SDLK_F4:           return MKEY_F4;
		case SDLK_F5:           return MKEY_F5;
		case SDLK_F6:           return MKEY_F6;
		case SDLK_F7:           return MKEY_F7;
		case SDLK_F8:           return MKEY_F8;
		case SDLK_F9:           return MKEY_F9;
		case SDLK_F10:          return MKEY_F10;
		case SDLK_F11:          return MKEY_F11;
		case SDLK_F12:          return MKEY_F12;

		default:                break;
	}

	return -1;
}

MAndroidWindow::MAndroidWindow(void) :
	m_focus(true),
	m_active(true),
	m_fullscreen(false),
	m_width(0),
	m_height(0),
	m_colorBits(0),
	m_pointerEvent(NULL)
{
	m_position[0] = 0;
	m_position[1] = 0;
}

MAndroidWindow::~MAndroidWindow(void)
{
	SDL_GL_DeleteContext(g_context);
	SDL_DestroyWindow(g_window);
	SDL_Quit();
}

void MAndroidWindow::setCursorPos(int x, int y)
{
	SDL_WarpMouseInWindow(g_window, x, y);
}

void MAndroidWindow::hideCursor(void)
{
	int r = SDL_ShowCursor(SDL_DISABLE);

	if (r < 0)
		fprintf(stderr, "SDL Error : %s\n", SDL_GetError());
}

void MAndroidWindow::showCursor(void)
{
	int r = SDL_ShowCursor(SDL_ENABLE);

	if (r < 0)
		fprintf(stderr, "SDL Error : %s\n", SDL_GetError());
}

void MAndroidWindow::setTitle(const char * title)
{
	SDL_SetWindowTitle(g_window, title);
}

void MAndroidWindow::setFullscreen(bool fullscreen)
{
	int r;

	if (fullscreen)
		r = SDL_SetWindowFullscreen(g_window, SDL_WINDOW_FULLSCREEN);
	else
		r = SDL_SetWindowFullscreen(g_window, SDL_DISABLE);

	if (r < 0)
		fprintf(stderr, "SDL Error : %s\n", SDL_GetError());

	m_fullscreen = fullscreen;
}

void MAndroidWindow::sendEvents(MWinEvent * event)
{
	MKeyboard * keyboard = MKeyboard::getInstance();
	MMouse * mouse = MMouse::getInstance();

	switch(event->type)
	{
		case MWIN_EVENT_KEY_DOWN:
			keyboard->onKeyDown(event->data[0]);
			break;

		case MWIN_EVENT_KEY_UP:
			keyboard->onKeyUp(event->data[0]);
			break;

		case MWIN_EVENT_WINDOW_RESIZE:
			m_width = (unsigned int)event->data[0];
			m_height = (unsigned int)event->data[1];
			break;

		case MWIN_EVENT_WINDOW_MOVE:
			m_position[0] = event->data[0];
			m_position[1] = event->data[1];
			break;

		case MWIN_EVENT_MOUSE_BUTTON_DOWN:
			mouse->downButton(event->data[0]);
			break;

		case MWIN_EVENT_MOUSE_BUTTON_UP:
			mouse->upButton(event->data[0]);
			break;

		case MWIN_EVENT_MOUSE_WHEEL_MOVE:
			mouse->setWheelDirection(event->data[0]);
			break;

		case MWIN_EVENT_MOUSE_MOVE:
			mouse->setPosition(event->data[0], event->data[1]);
			break;
	}

	if(m_pointerEvent)
		m_pointerEvent(event);
}

bool MAndroidWindow::isMouseOverWindow(void)
{
	MMouse * mouse = MMouse::getInstance();
	int x = mouse->getXPosition();
	int y = mouse->getYPosition();

	if(x >= 0 && y >= 0 && x < (int)getWidth() && y < (int)getHeight())
		return true;

	return false;
}

unsigned long MAndroidWindow::getSystemTick(void)
{
	return SDL_GetTicks();
}

bool MAndroidWindow::onEvents(void)
{
	MWinEvent mevent;
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
        switch (event.type)
		{
			case SDL_QUIT:
			{
				mevent.type = MWIN_EVENT_WINDOW_CLOSE;
				sendEvents(&mevent);
				break;
			}

			case SDL_WINDOWEVENT:
			{
                switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						mevent.type = MWIN_EVENT_WINDOW_RESIZE;
						mevent.data[0] = event.window.data1;
						mevent.data[1] = event.window.data2;
						sendEvents(&mevent);
                        break;
					case SDL_WINDOWEVENT_MOVED:
						mevent.type = MWIN_EVENT_WINDOW_MOVE;
						mevent.data[0] = event.window.data1;
						mevent.data[1] = event.window.data2;
						sendEvents(&mevent);
						break;
					case SDL_WINDOWEVENT_CLOSE:
						mevent.type = MWIN_EVENT_WINDOW_CLOSE;
						sendEvents(&mevent);
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						m_focus = true;
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						m_focus = false;
						break;
				}
				break;
			}

			// Keyboard
			case SDL_KEYDOWN:
			{
                // SDL_Log("SDL_KEYDOWN");
				int key = translateKey(event.key.keysym.sym);
				if(key > 0 && key < 256)
				{
					mevent.type = MWIN_EVENT_KEY_DOWN;
					mevent.data[0] = key;
					sendEvents(&mevent);
				}
				break;
			}
			case SDL_KEYUP:
			{
				int key = translateKey(event.key.keysym.sym);
				if(key > 0 && key < 256)
				{
					mevent.type = MWIN_EVENT_KEY_UP;
					mevent.data[0] = key;
					sendEvents(&mevent);
				}
				break;
			}
			case SDL_TEXTINPUT:
			{
				mevent.type = MWIN_EVENT_CHAR;
				mevent.data[0] = (int) event.text.text[0];
				sendEvents(&mevent);
				break;
			}

			// Mouse
			case SDL_MOUSEMOTION:
			{
				mevent.type = MWIN_EVENT_MOUSE_MOVE;
                mevent.data[0] = event.motion.x; // relative to window
                mevent.data[1] = event.motion.y; // relative to window
				sendEvents(&mevent);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
						mevent.data[0] = MMOUSE_BUTTON_LEFT;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_MIDDLE:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
						mevent.data[0] = MMOUSE_BUTTON_MIDDLE;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_RIGHT:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
						mevent.data[0] = MMOUSE_BUTTON_RIGHT;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_X1:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
						mevent.data[0] = MMOUSE_BUTTON_X1;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_X2:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_DOWN;
						mevent.data[0] = MMOUSE_BUTTON_X2;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
				}
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
						mevent.data[0] = MMOUSE_BUTTON_LEFT;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_MIDDLE:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
						mevent.data[0] = MMOUSE_BUTTON_MIDDLE;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_RIGHT:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
						mevent.data[0] = MMOUSE_BUTTON_RIGHT;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_X1:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
						mevent.data[0] = MMOUSE_BUTTON_X1;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
					case SDL_BUTTON_X2:
					{
						mevent.type = MWIN_EVENT_MOUSE_BUTTON_UP;
						mevent.data[0] = MMOUSE_BUTTON_X2;
						mevent.data[1] = event.button.x;
						mevent.data[2] = event.button.y;
						sendEvents(&mevent);
						break;
					}
				}
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				if (event.wheel.y > 0)
				{
					mevent.type = MWIN_EVENT_MOUSE_WHEEL_MOVE;
					mevent.data[0] = 1;
					sendEvents(&mevent);
				}
				else if (event.wheel.y < 0)
				{
					mevent.type = MWIN_EVENT_MOUSE_WHEEL_MOVE;
					mevent.data[0] = -1;
					sendEvents(&mevent);
				}
				break;
			}
			// Touch
			case SDL_FINGERDOWN:
			{
				mevent.type = MWIN_EVENT_FINGER_DOWN;
				mevent.data[0] = event.tfinger.fingerId;
				mevent.data[1] = INT_MAX * event.tfinger.x;
				mevent.data[2] = INT_MAX * event.tfinger.y;
				mevent.data[3] = INT_MAX * event.tfinger.pressure;
				sendEvents(&mevent);
				break;
			}
			case SDL_FINGERUP:
			{
				mevent.type = MWIN_EVENT_FINGER_UP;
				mevent.data[0] = event.tfinger.fingerId;
				mevent.data[1] = INT_MAX * event.tfinger.x;
				mevent.data[2] = INT_MAX * event.tfinger.y;
				mevent.data[3] = INT_MAX * event.tfinger.pressure;
				sendEvents(&mevent);
				break;
			}
			case SDL_FINGERMOTION:
			{
				mevent.type = MWIN_EVENT_FINGER_MOVE;
				mevent.data[0] = event.tfinger.fingerId;
				mevent.data[1] = INT_MAX * event.tfinger.dx;
				mevent.data[2] = INT_MAX * event.tfinger.dy;
				mevent.data[3] = INT_MAX * event.tfinger.pressure;
				sendEvents(&mevent);
				break;
			}

			// Default
			default:
				break;
		}
	}

	return true;
}

void MAndroidWindow::swapBuffer(void)
{
	SDL_GL_SwapWindow(g_window);
}

bool MAndroidWindow::create(const char * title, unsigned int width, unsigned int height, int colorBits, bool fullscreen)
{
	m_width = width;
	m_height = height;
	m_colorBits = colorBits;
	m_fullscreen = fullscreen;

	SDL_version compiled;
	SDL_version linked;

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	fprintf(stdout, "Info\t SDL compiled version : %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
	fprintf(stdout, "Info\t SDL linked version : %d.%d.%d\n", linked.major, linked.minor, linked.patch);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "SDL Error : %s\n", SDL_GetError());
		return false;
	}

	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL | SDL_PIXELFORMAT_RGBA8888;

	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	
	m_width = current.w;
	m_height = current.h;
	
	g_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, flags);

	if (!g_window)
	{
		MLOG_ERROR("SDL Error : " << SDL_GetError());
		return false;
	}

	g_context = SDL_GL_CreateContext(g_window);
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	

	return true;
}

void MAndroidWindow::sleep(double time)
{
	if (time <= 0)
		return;

	SDL_Delay(time);
}

#endif
