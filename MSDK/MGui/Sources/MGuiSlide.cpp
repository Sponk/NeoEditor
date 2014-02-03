/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiSlide.cpp
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


// constructor
MGuiSlide::MGuiSlide(MVector2 position, MVector2 scale, MVector4 color, void (* pointerEvent)(MGuiSlide * slide, MGuiEvent * guiEvent)):
m_value(0),
m_minValue(0),
m_maxValue(1)
{
	m_button.setPosition(position);
	m_button.setScale(scale);

	setDirection(MVector2(10, 0));

	setParentWindow(NULL);
	setPosition(position);
	setScale(getDirection() + scale);
	setNormalColor(color);
	setHighLightColor(color);
	setPressedColor(color);
	setPointerEvent(pointerEvent);
}

float MGuiSlide::getNormalizedValue(void)
{
	if(getVariableType())
	{
		if(getVariableType() == M_VAR_INT || getVariableType() == M_VAR_UINT)
		{
			int iValue;
			if(m_value >= 0)
				iValue = (int)(m_value + 0.5f);
			else
				iValue = (int)(m_value - 0.5f);

			return (iValue - getMinValue()) / (getMaxValue() - getMinValue());
		}
	}

	return (getValue() - getMinValue()) / (getMaxValue() - getMinValue());
}

void MGuiSlide::sendVariable(void)
{
	if(getVariablePointer())
	{
		switch(getVariableType())
		{
		case M_VAR_INT:
			{
				int * value = (int *)getVariablePointer();
				if(m_value >= 0)
					*value = (int)(m_value + 0.5f);
				else
					*value = (int)(m_value - 0.5f);
			}
			break;
		case M_VAR_UINT:
			{
				unsigned int * value = (unsigned int *)getVariablePointer();
				if(m_value >= 0)
					*value = (unsigned int)(m_value + 0.5f);
				else
					*value = 0;
			}
				break;
		case M_VAR_FLOAT:
			{
				float * value = (float *)getVariablePointer();
				*value = m_value;
			}
			break;
		}
	}

	// send on change gui event
	if(m_pointerEvent)
	{
		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_SEND_VARIABLE;

		m_pointerEvent(this, &guiEvent);
	}
}

void MGuiSlide::updateFromVariable(void)
{
	if(! getVariablePointer())
		return;

	switch(getVariableType())
	{
	case M_VAR_INT:
		{
			int * value = (int *)getVariablePointer();
			setValue((float)*value);
		}
		break;
	case M_VAR_UINT:
		{
			unsigned int * value = (unsigned int *)getVariablePointer();
			setValue((float)*value);
		}
		break;
	case M_VAR_FLOAT:
		{
			float * value = (float *)getVariablePointer();
			setValue(*value);
		}
		break;
	}
}

void MGuiSlide::onEvent(MWinEvent * windowEvent)
{
	MGuiWindow * parent = getParentWindow();

	MMouse * mouse = MMouse::getInstance();

	switch(windowEvent->type)
	{
	case MWIN_EVENT_MOUSE_WHEEL_MOVE:
	case MWIN_EVENT_MOUSE_MOVE:
		if(parent->isHighLight() && m_button.isPointInside(getMouseLocalPosition()) && (! mouse->isLeftButtonPushed()) && (! mouse->isMiddleButtonPushed()) && (! mouse->isRightButtonPushed()))
		{
			setHighLight(true);

			if(m_pointerEvent) // send gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_MOVE;
				guiEvent.data[0] = windowEvent->data[0];
				guiEvent.data[1] = windowEvent->data[1];

				m_pointerEvent(this, &guiEvent);
			}

			// break events
			if(parent->breakEvents())
				return;
		}
		else{
			setHighLight(false);
		}

		if(isPressed())
		{
			float value = getValueFromPoint(getMouseLocalPosition());
			setValue(value);
		}
		break;
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		if(isHighLight())
		{
			if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT){
				setPressed(true);
				m_offset = getMouseLocalPosition() - m_button.getPosition();
			}

			if(m_pointerEvent) // send gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_DOWN;
				guiEvent.data[0] = windowEvent->data[0];

				m_pointerEvent(this, &guiEvent);
			}
		}
		break;
	case MWIN_EVENT_MOUSE_BUTTON_UP:
		if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
		{
			setPressed(false);
		}
		break;
	}
}

void MGuiSlide::updatePosition(void)
{
	// update slide position
	m_button.setPosition(getPointfromValue(getValue()));
}

void MGuiSlide::onChange(void)
{
	// update slide position
	updatePosition();

	// send linked variable
	sendVariable();

	// send on change gui event
	if(m_pointerEvent)
	{
		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_ON_CHANGE;

		m_pointerEvent(this, &guiEvent);
	}
}

void MGuiSlide::setValue(float value)
{
	float old = m_value;

	m_value = value;

	// clamp value
	if(m_value < m_minValue)
	{
		m_value = m_minValue;
	}
	else if(m_value > m_maxValue)
	{
		m_value = m_maxValue;
	}

	if(m_value != old)
		onChange();
}

void MGuiSlide::setDirection(const MVector2 & direction)
{
	m_direction = direction;
	setScale(getDirection() + m_button.getScale());
}

float MGuiSlide::getValueFromPoint(const MVector2 & point)
{
	float pos;
	float val;

	if(getDirection().x > getDirection().y)
	{
		pos = getPosition().x + m_offset.x;
		val = (point.x - pos) / getDirection().x;
	}
	else
	{
		pos = getPosition().y + m_offset.y;
		val = (point.y - pos) / getDirection().y;
	}

	if(val < 0)
		val = 0;

	if(val > 1)
		val = 1;

	return getMinValue() + (val * (getMaxValue() - getMinValue()));
}

MVector2 MGuiSlide::getPointfromValue(float value)
{
	float nValue;
	MVector2 point;

	// variable pointer
	if(getVariablePointer())
	{
		if(getVariableType() == M_VAR_INT || getVariableType() == M_VAR_UINT)
		{
			int iValue;

			if(value >= 0)
				iValue = (int)(value+0.5f);
			else
				iValue = (int)(value-0.5f);

			nValue = (iValue - getMinValue()) / (getMaxValue() - getMinValue());

			if(nValue < 0)
				nValue = 0;

			if(nValue > 1)
				nValue = 1;

			point = getPosition() + (getDirection() * nValue);
			return point;
		}
	}

	// normal
	nValue = (value - getMinValue()) / (getMaxValue() - getMinValue());

	if(nValue < 0)
		nValue = 0;

	if(nValue > 1)
		nValue = 1;

	point = getPosition() + (getDirection() * nValue);
	return point;
}

void MGuiSlide::draw(void)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	if(! isVisible())
		return;

	if(! isPressed())
		updateFromVariable();

	if(isPressed()) // pressed
	{
		render->setColor4(getPressedColor());

		if(hasPressedTexture())
		{
			render->enableTexture();
			m_button.drawTexturedQuad(getPressedTexture());
		}
		else
		{
			render->disableTexture();
			m_button.drawQuad();
		}
	}
	else if(isHighLight()) // highLight
	{
		render->setColor4(getHighLightColor());

		if(hasHighLightTexture()){
			render->enableTexture();
			m_button.drawTexturedQuad(getHighLightTexture());
		}
		else
		{
			render->disableTexture();
			m_button.drawQuad();
		}
	}
	else // normal
	{
		render->setColor4(getNormalColor());

		if(hasNormalTexture())
		{
			render->enableTexture();
			m_button.drawTexturedQuad(getNormalTexture());
		}
		else
		{
			render->disableTexture();
			m_button.drawQuad();
		}
	}

	MGuiEvent guiEvent;
	guiEvent.type = MGUI_EVENT_DRAW;
	if(m_pointerEvent)
		m_pointerEvent(this, &guiEvent);
}
