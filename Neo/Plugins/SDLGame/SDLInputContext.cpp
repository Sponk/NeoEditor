#include "SDLInputContext.h"
#include <NeoEngine.h>
#include <SDL.h>
#include <algorithm>
#include <limits>

#ifdef max // Defined by the Windows SDK
#undef max
#endif

using namespace Neo;

INPUT_KEYS SDLInputContext::translateEvent(SDL_Keycode key)
{
	switch (key)
	{
		case SDLK_a:
			return KEY_A;
		case SDLK_b:
			return KEY_B;
		case SDLK_c:
			return KEY_C;
		case SDLK_d:
			return KEY_D;
		case SDLK_e:
			return KEY_E;
		case SDLK_f:
			return KEY_F;
		case SDLK_g:
			return KEY_G;
		case SDLK_h:
			return KEY_H;
		case SDLK_i:
			return KEY_I;
		case SDLK_j:
			return KEY_J;
		case SDLK_k:
			return KEY_K;
		case SDLK_l:
			return KEY_L;
		case SDLK_m:
			return KEY_M;
		case SDLK_n:
			return KEY_N;
		case SDLK_o:
			return KEY_O;
		case SDLK_p:
			return KEY_P;
		case SDLK_q:
			return KEY_Q;
		case SDLK_r:
			return KEY_R;
		case SDLK_s:
			return KEY_S;
		case SDLK_t:
			return KEY_T;
		case SDLK_u:
			return KEY_U;
		case SDLK_v:
			return KEY_V;
		case SDLK_w:
			return KEY_W;
		case SDLK_x:
			return KEY_X;
		case SDLK_y:
			return KEY_Y;
		case SDLK_z:
			return KEY_Z;

		case SDLK_KP_0:
			return KEY_KP0;
		case SDLK_KP_1:
			return KEY_KP1;
		case SDLK_KP_2:
			return KEY_KP2;
		case SDLK_KP_3:
			return KEY_KP3;
		case SDLK_KP_4:
			return KEY_KP4;
		case SDLK_KP_5:
			return KEY_KP5;
		case SDLK_KP_6:
			return KEY_KP6;
		case SDLK_KP_7:
			return KEY_KP7;
		case SDLK_KP_8:
			return KEY_KP8;
		case SDLK_KP_9:
			return KEY_KP9;

		case SDLK_0:
			return KEY_0;
		case SDLK_1:
			return KEY_1;
		case SDLK_2:
			return KEY_2;
		case SDLK_3:
			return KEY_3;
		case SDLK_4:
			return KEY_4;
		case SDLK_5:
			return KEY_5;
		case SDLK_6:
			return KEY_6;
		case SDLK_7:
			return KEY_7;
		case SDLK_8:
			return KEY_8;
		case SDLK_9:
			return KEY_9;

		case SDLK_KP_ENTER:
			return KEY_KP_ENTER;
		case SDLK_SPACE:
			return KEY_SPACE;
		case SDLK_ESCAPE:
			return KEY_ESCAPE;
		case SDLK_TAB:
			return KEY_TAB;
		case SDLK_LSHIFT:
			return KEY_LSHIFT;
		case SDLK_RSHIFT:
			return KEY_RSHIFT;
		case SDLK_LCTRL:
			return KEY_LCONTROL;
		case SDLK_RCTRL:
			return KEY_RCONTROL;
		case SDLK_LALT:
			return KEY_LALT;

		case SDLK_MODE:
		case SDLK_RALT:
			return KEY_RALT;
		case SDLK_LGUI:
			return KEY_LSUPER;
		case SDLK_RGUI:
			return KEY_RSUPER;
		case SDLK_MENU:
			return KEY_MENU;
		case SDLK_NUMLOCKCLEAR:
			return KEY_NUMLOCK;
		case SDLK_PAUSE:
			return KEY_PAUSE;
		case SDLK_DELETE:
			return KEY_DELETE;
		case SDLK_BACKSPACE:
			return KEY_BACKSPACE;
		case SDLK_RETURN:
			return KEY_RETURN;
		case SDLK_HOME:
			return KEY_HOME;
		case SDLK_END:
			return KEY_END;
		case SDLK_PAGEUP:
			return KEY_PAGEUP;
		case SDLK_PAGEDOWN:
			return KEY_PAGEDOWN;
		case SDLK_INSERT:
			return KEY_INSERT;

		case SDLK_LEFT:
			return KEY_LEFT_ARROW;
		case SDLK_RIGHT:
			return KEY_RIGHT_ARROW;
		case SDLK_DOWN:
			return KEY_DOWN_ARROW;
		case SDLK_UP:
			return KEY_UP_ARROW;

		case SDLK_F1:
			return KEY_F1;
		case SDLK_F2:
			return KEY_F2;
		case SDLK_F3:
			return KEY_F3;
		case SDLK_F4:
			return KEY_F4;
		case SDLK_F5:
			return KEY_F5;
		case SDLK_F6:
			return KEY_F6;
		case SDLK_F7:
			return KEY_F7;
		case SDLK_F8:
			return KEY_F8;
		case SDLK_F9:
			return KEY_F9;
		case SDLK_F10:
			return KEY_F10;
		case SDLK_F11:
			return KEY_F11;
		case SDLK_F12:
			return KEY_F12;

		default:
			break;
	}

	return KEY_DUMMY;
}

void SDLInputContext::handleInput()
{
	SDL_Event event;
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();

	Keyboard& kbd = getKeyboard();
	Mouse& mouse = getMouse();

	flush();
	
	// If the mouse is locked we need to ensure that the direction is reset when no movement happened.
	if(isMouseRelative())
		mouse.setDirection(Vector2(0,0));

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				engine->setActive(false);
				break;
			}

				// Keyboard
			case SDL_KEYDOWN:
			{
				kbd.keyDown(translateEvent(event.key.keysym.sym));
				break;
			}
			case SDL_KEYUP:
			{
				kbd.keyUp(translateEvent(event.key.keysym.sym));
				break;
			}
			case SDL_TEXTINPUT:
			{
				unsigned int character;
				strncpy((char*) &character, event.text.text, 4);
				kbd.setCharacter(character);
				break;
			}

			case SDL_MOUSEMOTION:
			{
				mouse.moveCursor(Vector2(event.motion.x, event.motion.y));
				
				if(isMouseRelative())
				{
					mouse.setDirection(Vector2(event.motion.xrel, event.motion.yrel));
				}
				
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
					{
						mouse.keyDown(MOUSE_BUTTON_LEFT);
						break;
					}
					case SDL_BUTTON_MIDDLE:
					{
						mouse.keyDown(MOUSE_BUTTON_MIDDLE);
						break;
					}
					case SDL_BUTTON_RIGHT:
					{
						mouse.keyDown(MOUSE_BUTTON_RIGHT);
						break;
					}
					case SDL_BUTTON_X1:
					{
						// TODO: Support for more than 3 mouse buttons!
						MLOG_INFO("This mouse button is not supported yet! FIXME!");
						break;
					}
					case SDL_BUTTON_X2:
					{
						MLOG_INFO("This mouse button is not supported yet! FIXME!");
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
						mouse.keyUp(MOUSE_BUTTON_LEFT);
						break;
					}
					case SDL_BUTTON_MIDDLE:
					{
						mouse.keyUp(MOUSE_BUTTON_MIDDLE);
						break;
					}
					case SDL_BUTTON_RIGHT:
					{
						mouse.keyUp(MOUSE_BUTTON_RIGHT);
						break;
					}
					case SDL_BUTTON_X1:
					{
						// TODO: Support for more than 3 mouse buttons!
						MLOG_INFO("This mouse button is not supported yet! FIXME!");
						break;
					}
					case SDL_BUTTON_X2:
					{
						MLOG_INFO("This mouse button is not supported yet! FIXME!");
						break;
					}
				}
				break;
			}
			case SDL_MOUSEWHEEL:
			{
				mouse.setScrollValue(mouse.getScrollValue() + event.wheel.y);
				break;
			}

#ifndef EMSCRIPTEN
			case SDL_CONTROLLERDEVICEADDED:
			{
				MLOG_INFO("Controller " << event.cdevice.which << " connected");
				addGameController(event.cdevice.which);
				break;
			}
			case SDL_CONTROLLERDEVICEREMOVED:
			{
				MLOG_INFO("Controller " << event.cdevice.which << " disconnected");
				removeGameController(event.cdevice.which);
				break;
			}
			case SDL_CONTROLLERAXISMOTION:
			{
				auto controller = m_devices[event.caxis.which];
				controller->setAxis(event.caxis.axis,
									static_cast<float>(event.caxis.value)
										/ static_cast<float>(std::numeric_limits<int16_t>().max()));
				break;
			}
			case SDL_CONTROLLERBUTTONDOWN:
			{
				auto controller = m_devices[event.cbutton.which];
				controller->keyDown(event.cbutton.button);
				break;
			}
			case SDL_CONTROLLERBUTTONUP:
			{
				auto controller = m_devices[event.cbutton.which];
				controller->keyUp(event.cbutton.button);
				break;
			}
			/*case SDL_JOYAXISMOTION:
			{
				mevent.type = MWIN_EVENT_JOYSTICK_MOVE;
				mevent.data[0] = event.jaxis.which;
				mevent.data[1] = event.jaxis.axis;
				mevent.data[2] = event.jaxis.value;
				sendEvents(&mevent);
				break;
			}
			case SDL_JOYBALLMOTION:
			{
				mevent.type = MWIN_EVENT_JOYSTICK_BALL_MOVE;
				mevent.data[0] = event.jball.which;
				mevent.data[1] = event.jball.ball;
				mevent.data[2] = event.jball.xrel;
				mevent.data[3] = event.jball.yrel;
				sendEvents(&mevent);
				break;
			}
			case SDL_JOYHATMOTION:
			{
				mevent.type = MWIN_EVENT_JOYSTICK_HAT_MOVE;
				mevent.data[0] = event.jhat.which;
				mevent.data[1] = event.jhat.hat;
				mevent.data[2] = event.jhat.value;
				sendEvents(&mevent);
				break;
			}
			case SDL_JOYBUTTONDOWN:
			{
				mevent.type = MWIN_EVENT_JOYSTICK_BUTTON_DOWN;
				mevent.data[0] = event.jbutton.which;
				mevent.data[1] = event.jbutton.button;
				sendEvents(&mevent);
				break;
			}
			case SDL_JOYBUTTONUP:
			{
				mevent.type = MWIN_EVENT_JOYSTICK_BUTTON_UP;
				mevent.data[0] = event.jbutton.which;
				mevent.data[1] = event.jbutton.button;
				sendEvents(&mevent);
				break;
			}
			case SDL_CONTROLLERDEVICEREMAPPED:
			{
				mevent.type = MWIN_EVENT_CONTROLLER_REMAPPED;
				mevent.data[0] = event.cdevice.which;
				sendEvents(&mevent);
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
			}*/
#endif

			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						engine->getSystemContext()->setScreenSize(event.window.data1, event.window.data2);
						break;

					case SDL_WINDOWEVENT_MOVED: // TODO: Interface in SystemContext!
						/*mevent.type = MWIN_EVENT_WINDOW_MOVE;
						mevent.data[0] = event.window.data1;
						mevent.data[1] = event.window.data2;
						sendEvents(&mevent);*/
						break;
					case SDL_WINDOWEVENT_CLOSE:
						/*
						mevent.type = MWIN_EVENT_WINDOW_CLOSE;
						sendEvents(&mevent);*/
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						engine->getInputContext()->getKeyboard().keyDown(WINDOW_SELECT);
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						engine->getInputContext()->getKeyboard().keyUp(WINDOW_SELECT);
						break;
				}
				break;
			}

				// Default
			default:
				break;
		}
	}

	if(!isMouseRelative()) // Only calculate direction from position when it is needed
		mouse.flushDirection();
}

#ifndef EMSCRIPTEN
int SDLInputContext::addGameController(int index)
{
	auto device = SDL_GameControllerOpen(index);
	if (!device)
	{
		MLOG_ERROR("SDL Error : " << SDL_GetError());
		return -1;
	}

	auto joy = SDL_GameControllerGetJoystick(device);
	auto id = SDL_JoystickInstanceID(joy);
	auto controller = make_shared<SDLController>(SDL_JoystickNumAxes(joy), SDL_JoystickNumButtons(joy), id, device);

	m_controllers.push_back(controller);
	m_devices[id] = controller;

	return id;
}

int SDLInputContext::removeGameController(int id)
{
	for (int i = 0; i < m_controllers.size(); ++i)
	{
		auto controller = static_pointer_cast<SDLController>(m_controllers[i]);
		if(controller->id == id && SDL_GameControllerGetAttached(controller->device))
		{
			SDL_GameControllerClose(controller->device);

			m_devices.erase(controller->id);
			m_controllers.erase(m_controllers.begin() + i);
			return id;
		}
	}
	return -1;
}
#endif

void SDLInputContext::setMouseRelative(bool value)
{
	// TODO: Error checking
	SDL_SetRelativeMouseMode((value) ? SDL_TRUE : SDL_FALSE);
	InputContext::setMouseRelative(value);
}
