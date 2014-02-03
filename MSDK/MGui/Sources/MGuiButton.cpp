/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiButton.cpp
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


#include "../Includes/MGui.h"


//constructor
MGuiButton::MGuiButton(const MVector2 & position, const MVector2 & scale, const MVector4 & color, void (* pointerEvent)(MGuiButton * button, MGuiEvent * guiEvent)):
m_mode(MGUI_BUTTON_SIMPLE),
m_isActive(false),
m_groupId(0)
{
	setText("");
	setTextSize(16);
	setParentWindow(NULL);
	setPosition(position);
	setScale(scale);
	setNormalColor(color);
	setHighLightColor(color);
	setPressedColor(color);
	setTextColor(MVector4(0, 0, 0, 0.5f));
	setPointerEvent(pointerEvent);
}

void MGuiButton::sendVariable(void)
{
	if(getVariablePointer())
	switch(getVariableType())
	{
	case M_VAR_BOOL:
		{
			bool * value = (bool *)getVariablePointer();
			*value = isPressed();
		}
		break;
            
        default:
            break;
	}
}

void MGuiButton::updateFromVariable(void)
{
	if(getVariablePointer())
	switch(getVariableType())
	{
	case M_VAR_BOOL:
		{
			bool * value = (bool *)getVariablePointer();
			if(*value)
				m_isPressed = true;
			else
				m_isPressed = false;
		}
            break;
            
        default:
            break;
	}
}

void MGuiButton::onChange(void)
{
	sendVariable();

	// send on change gui event
	if(m_pointerEvent)
	{
		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_ON_CHANGE;

		m_pointerEvent(this, &guiEvent);
	}
}

void MGuiButton::press(bool press)
{
	if(press != isPressed())
	{
		MGui2d::setPressed(press);
		onChange();
	}	
}

void MGuiButton::setPressed(bool pressed)
{
	if(pressed != isPressed())
	{
		MGui2d::setPressed(pressed);
		sendVariable();
	}
}

void MGuiButton::onEvent(MWinEvent * windowEvent)
{
	MGuiWindow * parent = getParentWindow();

	// events
	switch(windowEvent->type)
	{
	case MWIN_EVENT_MOUSE_WHEEL_MOVE:
	case MWIN_EVENT_MOUSE_MOVE:
		if(parent->isHighLight() && isMouseInside())
		{
			setHighLight(true);
			if((! isCheckButton()) && (!isPressed()) && m_isActive)
			{
				press(true);
			}

			if(m_pointerEvent) // send mouse move gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_MOVE;
				guiEvent.data[0] = windowEvent->data[0];
				guiEvent.data[1] = windowEvent->data[1];

				m_pointerEvent(this, &guiEvent);
			}
		}
		else
		{
		    setHighLight(false);
			if((! isCheckButton()) && (! isGroupButton()) && isPressed())
			{
				press(false);
			}
		}
		break;
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		if(parent->isHighLight() && isMouseInside())
		{
			if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
			{
				if(! isCheckButton())
				{
					if(isGroupButton())
						 unPressGroupButtons();
                    press(true);
				}

				if(! isHighLight())
                    setHighLight(true);
			}

			m_isActive = true;

			if(m_pointerEvent) // send mouse button down gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_DOWN;
				guiEvent.data[0] = windowEvent->data[0];

				m_pointerEvent(this, &guiEvent);
			}
		}

		break;
	case MWIN_EVENT_MOUSE_BUTTON_UP:
		if(parent->isHighLight() && isMouseInside())
		{
			if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
			{
			    if(m_isActive)
			    {
                    if(isCheckButton())
                    {
                        press(! isPressed());
                    }
                }

				if(! isHighLight())
                    setHighLight(true);
			}

			if(m_pointerEvent) // send mouse button up gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_UP;
				guiEvent.data[0] = windowEvent->data[0];

				m_pointerEvent(this, &guiEvent);
			}

			// break events
			if(parent->breakEvents())
				return;

			if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
			{
				if((! isCheckButton()) && (! isGroupButton()))
					press(false);
			}
		}

		m_isActive = false;

            break;
            
        default:
            break;
	}
}

bool MGuiButton::unPressGroupButtons()
{
	MGuiWindow * parent = getParentWindow();

	if(parent)
	{
		unsigned int i;
		unsigned int bSize = parent->getButtonsNumber();
		for(i=0; i<bSize; i++)
		{
			MGuiButton * button = parent->getButton(i);

			if(button->isGroupButton())
			if(button->getGroupId() == getGroupId())
			{
				button->press(false);
			}
		}

		return true;
	}

	return false;
}

void MGuiButton::draw(void)
{
	if(! isVisible())
		return;

	updateFromVariable();

	MGui2d::draw();

	MGuiEvent guiEvent;
	guiEvent.type = MGUI_EVENT_DRAW;
	if(m_pointerEvent)
		m_pointerEvent(this, &guiEvent);
}
