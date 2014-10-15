/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MGameWinEvents.h
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


#ifndef _M_GAME_WIN_EVENTS_H
#define _M_GAME_WIN_EVENTS_H

#include <MInputContext.h>
#include <MWindow.h>
#include <MWinEvents.h>

#include <limits.h>


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
	static int joymap[4] = {-1, -1, -1, -1};

	MWindow * window = MWindow::getInstance();
	MInputContext * input = MEngine::getInstance()->getInputContext();
	
	switch(windowEvents->type)
	{
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		switch(windowEvents->data[0])
		{
		case MMOUSE_BUTTON_LEFT:
			input->downKey("MOUSE_BUTTON_LEFT");
			break;
		case MMOUSE_BUTTON_MIDDLE:
			input->downKey("MOUSE_BUTTON_MIDDLE");
			break;
		case MMOUSE_BUTTON_RIGHT:
			input->downKey("MOUSE_BUTTON_RIGHT");
			break;
		case MMOUSE_BUTTON_X1:
			input->downKey("MOUSE_BUTTON_X1");
			break;
		case MMOUSE_BUTTON_X2:
			input->downKey("MOUSE_BUTTON_X2");
		}
		break;

	case MWIN_EVENT_MOUSE_BUTTON_UP:
		switch(windowEvents->data[0])
		{
		case MMOUSE_BUTTON_LEFT:
			input->upKey("MOUSE_BUTTON_LEFT");
			break;
		case MMOUSE_BUTTON_MIDDLE:
			input->upKey("MOUSE_BUTTON_MIDDLE");
			break;
		case MMOUSE_BUTTON_RIGHT:
			input->upKey("MOUSE_BUTTON_RIGHT");
			break;
		case MMOUSE_BUTTON_X1:
			input->upKey("MOUSE_BUTTON_X1");
			break;
		case MMOUSE_BUTTON_X2:
			input->upKey("MOUSE_BUTTON_X2");
		}
		break;

	case MWIN_EVENT_MOUSE_MOVE:
		input->setAxis("MOUSE_X", (float)(windowEvents->data[0] / (float)window->getWidth()));
		input->setAxis("MOUSE_Y", (float)(windowEvents->data[1] / (float)window->getHeight()));
		break;

	case MWIN_EVENT_MOUSE_WHEEL_MOVE:
		input->setAxis("MOUSE_WHEEL", (float)windowEvents->data[0]);
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

    case MWIN_EVENT_JOYSTICK_ADDED:
	case MWIN_EVENT_CONTROLLER_ADDED:
		{
			for (int i = 0; i < 4; ++i)
			{
				if (joymap[i] == -1)
				{
					joymap[i] = windowEvents->data[0];
					break;
				}
			}
		}
		break;

    case MWIN_EVENT_JOYSTICK_REMOVED:
	case MWIN_EVENT_CONTROLLER_REMOVED:
		{
			for (int i = 0; i < 4; ++i)
			{
				if (joymap[i] == windowEvents->data[0])
				{
					joymap[i] = -1;
					break;
				}
			}
		}
		break;       

    case MWIN_EVENT_JOYSTICK_MOVE:
	case MWIN_EVENT_CONTROLLER_MOVE:
		{
			int joy = -1;
			for (int i = 0; i < 4; ++i)
			{
				if (joymap[i] == windowEvents->data[0])
				{
					joy = i;
					break;
				}
			}

			if (joy == -1)
				break;

			char name[32];
			switch (windowEvents->data[1])
			{
				case MCONTROLLER_AXIS_LEFTX:
					sprintf(name, "JOY%d_AXIS_LEFTX", joy+1);
                    input->setAxis(name, (float) windowEvents->data[2] / SHRT_MAX);
					break;
				case MCONTROLLER_AXIS_LEFTY:
					sprintf(name, "JOY%d_AXIS_LEFTY", joy+1);
                    input->setAxis(name, (float) windowEvents->data[2] / SHRT_MAX);
					break;
				case MCONTROLLER_AXIS_RIGHTX:
					sprintf(name, "JOY%d_AXIS_RIGHTX", joy+1);
                    input->setAxis(name, (float) windowEvents->data[2] / SHRT_MAX);
					break;
				case MCONTROLLER_AXIS_RIGHTY:
					sprintf(name, "JOY%d_AXIS_RIGHTY", joy+1);
                    input->setAxis(name, (float) windowEvents->data[2] / SHRT_MAX);
					break;
				case MCONTROLLER_AXIS_TRIGGERLEFT:
					sprintf(name, "JOY%d_AXIS_TRIGGERLEFT", joy+1);
                    input->setAxis(name, (float) windowEvents->data[2] / SHRT_MAX);
					break;
				case MCONTROLLER_AXIS_TRIGGERRIGHT:
					sprintf(name, "JOY%d_AXIS_TRIGGERRIGHT", joy+1);
                    input->setAxis(name, (float) windowEvents->data[2] / SHRT_MAX);
					break;
			}
		}
		break;

    case MWIN_EVENT_JOYSTICK_BUTTON_DOWN:
	case MWIN_EVENT_CONTROLLER_BUTTON_DOWN:
		{
			int joy = -1;
			for (int i = 0; i < 4; ++i)
			{
				if (joymap[i] == windowEvents->data[0])
				{
					joy = i;
					break;
				}
			}

			if (joy == -1)
				break;

			char name[32];
			switch (windowEvents->data[1])
			{
				case MCONTROLLER_BUTTON_A:
					sprintf(name, "JOY%d_BUTTON_A", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_B:
					sprintf(name, "JOY%d_BUTTON_B", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_X:
					sprintf(name, "JOY%d_BUTTON_X", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_Y:
					sprintf(name, "JOY%d_BUTTON_Y", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_BACK:
					sprintf(name, "JOY%d_BUTTON_BACK", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_GUIDE:
					sprintf(name, "JOY%d_BUTTON_GUIDE", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_START:
					sprintf(name, "JOY%d_BUTTON_START", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_LEFTSTICK:
					sprintf(name, "JOY%d_BUTTON_LEFTSTICK", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_RIGHTSTICK:
					sprintf(name, "JOY%d_BUTTON_RIGHTSTICK", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_LEFTSHOULDER:
					sprintf(name, "JOY%d_BUTTON_LEFTSHOULDER", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_RIGHTSHOULDER:
					sprintf(name, "JOY%d_BUTTON_RIGHTSHOULDER", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADUP:
                    sprintf(name, "JOY%d_BUTTON_DPADUP", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADDOWN:
                    sprintf(name, "JOY%d_BUTTON_DPADDOWN", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADLEFT:
                    sprintf(name, "JOY%d_BUTTON_DPADLEFT", joy+1);
					input->downKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADRIGHT:
                    sprintf(name, "JOY%d_BUTTON_DPADRIGHT", joy+1);
					input->downKey(name);
					break;
			}
		}
		break;

    case MWIN_EVENT_JOYSTICK_BUTTON_UP:
	case MWIN_EVENT_CONTROLLER_BUTTON_UP:
		{
			int joy = -1;
			for (int i = 0; i < 4; ++i)
			{
				if (joymap[i] == windowEvents->data[0])
				{
					joy = i;
					break;
				}
			}

			if (joy == -1)
				break;

			char name[32];
			switch (windowEvents->data[1])
			{
				case MCONTROLLER_BUTTON_A:
					sprintf(name, "JOY%d_BUTTON_A", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_B:
					sprintf(name, "JOY%d_BUTTON_B", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_X:
					sprintf(name, "JOY%d_BUTTON_X", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_Y:
					sprintf(name, "JOY%d_BUTTON_Y", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_BACK:
					sprintf(name, "JOY%d_BUTTON_BACK", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_GUIDE:
					sprintf(name, "JOY%d_BUTTON_GUIDE", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_START:
					sprintf(name, "JOY%d_BUTTON_START", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_LEFTSTICK:
					sprintf(name, "JOY%d_BUTTON_LEFTSTICK", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_RIGHTSTICK:
					sprintf(name, "JOY%d_BUTTON_RIGHTSTICK", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_LEFTSHOULDER:
					sprintf(name, "JOY%d_BUTTON_LEFTSHOULDER", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_RIGHTSHOULDER:
					sprintf(name, "JOY%d_BUTTON_RIGHTSHOULDER", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADUP:
                    sprintf(name, "JOY%d_BUTTON_DPADUP", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADDOWN:
                    sprintf(name, "JOY%d_BUTTON_DPADDOWN", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADLEFT:
                    sprintf(name, "JOY%d_BUTTON_DPADLEFT", joy+1);
					input->upKey(name);
					break;
				case MCONTROLLER_BUTTON_DPADRIGHT:
                    sprintf(name, "JOY%d_BUTTON_DPADRIGHT", joy+1);
					input->upKey(name);
					break;
			}
		}
		break;

	case MWIN_EVENT_FINGER_DOWN:
		input->beginTouch(windowEvents->data[0], MVector2(
					(float) windowEvents->data[1] / INT_MAX,
					(float) windowEvents->data[2] / INT_MAX
				));
		break;

	case MWIN_EVENT_FINGER_UP:
		input->endTouch(windowEvents->data[0], MVector2(
					(float) windowEvents->data[1] / INT_MAX,
					(float) windowEvents->data[2] / INT_MAX
				));
		break;

	case MWIN_EVENT_FINGER_MOVE:
		input->updateTouch(windowEvents->data[0], input->getLastTouchPosition(windowEvents->data[0]) + MVector2(
					(float) windowEvents->data[1] / INT_MAX,
					(float) windowEvents->data[2] / INT_MAX
				));
		break;
	}
}

#endif
