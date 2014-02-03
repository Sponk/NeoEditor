/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MGameWinEvents.h
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


#ifndef _M_GAME_WIN_EVENTS_H
#define _M_GAME_WIN_EVENTS_H


const char * getKeyName(unsigned int key)
{
	// ascii
	if(key >= 65 && key <= 90)
	{
		static char name[2] = {0, 0};
		name[0] = (char)key;
		return name;
	}

	// keyboard keys
	switch(key)
	{
	case MKEY_BACKSPACE: return "BACKSPACE";
	case MKEY_TAB:		 return "TAB";
	case MKEY_ESCAPE:	 return "ESCAPE";
	case MKEY_SPACE:	 return "SPACE";
	case MKEY_DELETE:	 return "DELETE";

	case MKEY_KP_ENTER:
	case MKEY_RETURN:	 return "ENTER";
	case MKEY_UP:		 return "UP";
	case MKEY_DOWN:		 return "DOWN";
	case MKEY_LEFT:		 return "LEFT";
	case MKEY_RIGHT:	 return "RIGHT";
	case MKEY_F1:		 return "F1";
	case MKEY_F2:		 return "F2";
	case MKEY_F3:		 return "F3";
	case MKEY_F4:		 return "F4";
	case MKEY_F5:		 return "F5";
	case MKEY_F6:		 return "F6";
	case MKEY_F7:		 return "F7";
	case MKEY_F8:		 return "F8";
	case MKEY_F9:		 return "F9";
	case MKEY_F10:		 return "F10";
	case MKEY_F11:		 return "F11";
	case MKEY_F12:		 return "F12";
	case MKEY_RSHIFT:	 return "RSHIFT";
	case MKEY_LSHIFT:	 return "LSHIFT";
	case MKEY_RCONTROL:	 return "RCONTROL";
	case MKEY_LCONTROL:	 return "LCONTROL";
	case MKEY_RALT:		 return "RALT";
	case MKEY_LALT:		 return "LALT";

	case MKEY_KP0: return "0";
	case MKEY_KP1: return "1";
	case MKEY_KP2: return "2";
	case MKEY_KP3: return "3";
	case MKEY_KP4: return "4";
	case MKEY_KP5: return "5";
	case MKEY_KP6: return "6";
	case MKEY_KP7: return "7";
	case MKEY_KP8: return "8";
	case MKEY_KP9: return "9";
	}

	return NULL;
}

void gameWinEvents(MWinEvent * windowEvents)
{
	MWindow * window = MWindow::getInstance();
	MInputContext * input = MEngine::getInstance()->getInputContext();
	MMouse * mouse = MMouse::getInstance();

	
	switch(windowEvents->type)
	{
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		switch(windowEvents->data[0])
		{
		case MMOUSE_BUTTON_LEFT:
			input->downKey("MOUSE_BUTTON1");
			break;
		case MMOUSE_BUTTON_MIDDLE:
			input->downKey("MOUSE_BUTTON2");
			break;
		case MMOUSE_BUTTON_RIGHT:
			input->downKey("MOUSE_BUTTON3");
			break;
		}
		break;

	case MWIN_EVENT_MOUSE_BUTTON_UP:
		switch(windowEvents->data[0])
		{
		case MMOUSE_BUTTON_LEFT:
			input->upKey("MOUSE_BUTTON1");
			break;
		case MMOUSE_BUTTON_MIDDLE:
			input->upKey("MOUSE_BUTTON2");
			break;
		case MMOUSE_BUTTON_RIGHT:
			input->upKey("MOUSE_BUTTON3");
			break;
		}
		break;

	case MWIN_EVENT_MOUSE_MOVE:
		input->setAxis("MOUSE_X", (float)(mouse->getXPosition() / (float)window->getWidth()));
		input->setAxis("MOUSE_Y", (float)(mouse->getYPosition() / (float)window->getHeight()));
		break;

	case MWIN_EVENT_MOUSE_WHEEL_MOVE:
		input->setAxis("MOUSE_WHEEL", (float)mouse->getWheelDirection());
		break;
			
	case MWIN_EVENT_KEY_DOWN:
		{
			unsigned int key = windowEvents->data[0];
			const char * name = getKeyName(key);
			if(name)
				input->downKey(name);
		}
		break;

	case MWIN_EVENT_KEY_UP:
		{
			unsigned int key = windowEvents->data[0];
			const char * name = getKeyName(key);
			if(name)
				input->upKey(name);
		}
		break;

	case MWIN_EVENT_JOYSTICK1_UPDATE:
		{
			MJoystick * joystick = window->getJoystick1();

			char name[32];
			for(int i=0; i<8; i++)
			{
				if(joystick->onKeyDown(i))
				{
					sprintf(name, "JOY1_BUTTON%d", i+1);
					input->downKey(name);
				}
				else if(joystick->onKeyUp(i))
				{
					sprintf(name, "JOY1_BUTTON%d", i+1);
					input->upKey(name);
				}
			}

			input->setAxis("JOY1_X", joystick->getX());
			input->setAxis("JOY1_Y", joystick->getY());
			input->setAxis("JOY1_Z", joystick->getZ());
			input->setAxis("JOY1_R", joystick->getR());
			input->setAxis("JOY1_U", joystick->getU());
			input->setAxis("JOY1_V", joystick->getV());
		}
		break;

	case MWIN_EVENT_JOYSTICK2_UPDATE:
		{
			MJoystick * joystick = window->getJoystick2();

			char name[32];
			for(int i=0; i<8; i++)
			{
				if(joystick->onKeyDown(i))
				{
					sprintf(name, "JOY2_BUTTON%d", i+1);
					input->downKey(name);
				}
				else if(joystick->onKeyUp(i))
				{
					sprintf(name, "JOY2_BUTTON%d", i+1);
					input->upKey(name);
				}
			}

			input->setAxis("JOY2_X", joystick->getX());
			input->setAxis("JOY2_Y", joystick->getY());
			input->setAxis("JOY2_Z", joystick->getZ());
			input->setAxis("JOY2_R", joystick->getR());
			input->setAxis("JOY2_U", joystick->getU());
			input->setAxis("JOY2_V", joystick->getV());
		}
		break;
	}
}

#endif