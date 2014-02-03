/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiEditText.cpp
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


#include <stdio.h>
#include "../Includes/MGui.h"


//constructor
MGuiEditText::MGuiEditText(const char * text, const MVector2 & position, float size, const MVector4 & color, void (* pointerEvent)(MGuiEditText * editText, MGuiEvent * guiEvent)):
m_charId(0),
m_startSelectionId(0),
m_endSelectionId(0),
m_limitLength(false),
m_maxLength(0),
m_isSingleLine(false)
{
	setParentWindow(NULL);
	setText(text);
	setPosition(position);
	setTextSize(size);
	setNormalColor(color);
	setHighLightColor(color);
	setPressedColor(color);
	setPointerEvent(pointerEvent);

	autoScaleFromText();
}

void MGuiEditText::sendVariable(void)
{
	if(getVariablePointer())
	{
		switch(getVariableType())
		{
		case M_VAR_BOOL:
			{
				bool * value = (bool *)getVariablePointer();

				int i;
				sscanf(getText(), "%d", &i);

				if(i == 1)
					*value = true;
				else if(i == 0)
					*value = false;
			}
			break;
		case M_VAR_INT:
			{
				int * value = (int *)getVariablePointer();

				int i;
				sscanf(getText(), "%d", &i);
				//if(i == (*value))
				//	return;

				*value = i;
			}
			break;
		case M_VAR_UINT:
			{
				unsigned int * value = (unsigned int *)getVariablePointer();

				int i;
				sscanf(getText(), "%d", &i);
				if(i < 0) i = 0;
				//if((unsigned int)i == (*value))
				//	return;

				*value = (unsigned int)i;
			}
			break;
		case M_VAR_FLOAT:
			{
				float * value = (float *)getVariablePointer();

				float f;
				sscanf(getText(), "%f", &f);
				//if(f == (*value))
				//	return;

				*value = f;
			}
			break;
		case M_VAR_STRING:
			{
				MString * value = (MString *)getVariablePointer();
				value->set(getText());
			}
			break;
		}

		unsigned int tSize = m_text.size();
		if(getCharId() > tSize)
			setCharId(tSize);
	}

	// send on change gui event
	if(m_pointerEvent)
	{
		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_SEND_VARIABLE;
		m_pointerEvent(this, &guiEvent);
	}
}

void MGuiEditText::updateFromVariable(void)
{
	if(! getVariablePointer())
		return;

	switch(getVariableType())
	{
	case M_VAR_BOOL:
		{
			bool * value = (bool *)getVariablePointer();

			if(*value)
				setText("1");
			else
				setText("0");
		}
		break;
	case M_VAR_INT:
		{
			int * value = (int *)getVariablePointer();

			char text[256];
			sprintf(text, "%d", *value);

			setText(text);
		}
		break;
	case M_VAR_UINT:
		{
			unsigned int * value = (unsigned int *)getVariablePointer();

			char text[256];
			sprintf(text, "%d", *value);

			setText(text);
		}
		break;
	case M_VAR_FLOAT:
		{
			float * value = (float *)getVariablePointer();

			char text[256];
			sprintf(text, "%0.2f", *value);

			setText(text);
		}
		break;
	case M_VAR_STRING:
		{
			MString * value = (MString *)getVariablePointer();
			setText(value->getSafeString());
		}
		break;
	}
}

void MGuiEditText::setText(const char * text)
{
	m_text = text;

	if(m_limitLength && m_text.size() > m_maxLength)
	{
		unsigned int sStart = m_maxLength;
		unsigned int sEnd = m_text.size();
		m_text.erase(m_text.begin() + sStart, m_text.begin() + sEnd);
	}

	unsigned int tSize = m_text.size();
	if(getCharId() > tSize)
		setCharId(tSize);

	autoScaleFromText();
}

void MGuiEditText::setPressed(bool pressed)
{
	MGui2d::setPressed(pressed);
	if(! pressed)
		setSelection(0, 0);
}

void MGuiEditText::autoScrolling(void)
{
	MGuiWindow * parent = getParentWindow();

	MVector2 newScroll = parent->getScroll();
	MVector2 charPos;

	// min, max window (with scroll)
	MVector2 min = - parent->getScroll();
	MVector2 max = parent->getScale() - parent->getScroll();

	// selection
	unsigned int sIds[2];
	bool selection = getSelectionIds(&sIds[0], &sIds[1]);

	if(selection)
	{
		// charPos
		charPos = getFont()->getCharacterPosition(
				getText(),
				getPosition(),
				getTextSize(),
				m_endSelectionId
		);
	}
	else
	{
		// charPos
		charPos = getFont()->getCharacterPosition(
				getText(),
				getPosition(),
				getTextSize(),
				getCharId()
		);
	}

	// new scrolling
	if(charPos.x > max.x) newScroll.x -= charPos.x - max.x + getTextSize()*0.2f;
	if((charPos.y + getTextSize()) > max.y) newScroll.y -= (charPos.y + getTextSize()) - max.y;

	if(charPos.x < min.x) newScroll.x += min.x - charPos.x;
	if(charPos.y < min.y) newScroll.y += min.y - charPos.y;

	parent->setScroll(newScroll);
	parent->resizeScroll();
	parent->limitScroll();
}

void MGuiEditText::upCharId(int direction)
{
	MVector2 pos = getFont()->getCharacterPosition(getText(), getPosition(), getTextSize(), getCharId());
	pos.x += 0.1f;
	pos.y += 0.1f;
	pos += MVector2(0, getTextSize() * direction);

	if(pos.y < getPosition().y)
		return;

	if(pos.y > (getPosition().y + getScale().y))
		return;

	unsigned int id = getFont()->findPointedCharacter(getText(), getPosition(), getTextSize(), pos);

	setCharId(id);
}

void MGuiEditText::setCharId(unsigned int id)
{
	if(id <= m_text.size())
		m_charId = id;
}

void MGuiEditText::addCharId(void)
{
	if(m_charId < m_text.size())
		m_charId ++;
}

void MGuiEditText::subCharId(void)
{
	if(m_charId > 0)
		m_charId --;
}

void MGuiEditText::autoScaleFromText(void)
{
	MGui2d::autoScaleFromText();

	if(getScale().x == 0)
		setScale(MVector2(getTextSize() * 10.0f, getTextSize()));
}

void MGuiEditText::onChange(void)
{
	// send on change gui event
	if(m_pointerEvent)
	{
		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_ON_CHANGE;

		m_pointerEvent(this, &guiEvent);
	}
}

bool MGuiEditText::getSelectionIds(unsigned int * start, unsigned int * end)
{
	if(m_endSelectionId > m_startSelectionId)
	{
		*start = m_startSelectionId;
		*end = m_endSelectionId;
	}
	else
	{
		*start = m_endSelectionId;
		*end = m_startSelectionId;
	}

	unsigned int tLength = strlen(getText());

	if(*start >= tLength)
		*start = tLength;

	if(*end >= tLength)
		*end = tLength;

	if(*start == *end)
		return false;

	return true;
}

bool MGuiEditText::canAddCharacter(void)
{
	if(! m_limitLength)
		return true;

	if(m_text.size() < m_maxLength)
		return true;

	return false;
}

void MGuiEditText::editText(MWinEvent * windowEvent)
{
	unsigned int sStart;
	unsigned int sEnd;

	bool selection = getSelectionIds(&sStart, &sEnd);

	if(selection)
		setCharId(sStart);

	// events
	if(windowEvent->type == MWIN_EVENT_KEY_DOWN)
	{
		switch(windowEvent->data[0])
		{
		case MKEY_UP:
			if(! isSingleLine())
				upCharId(-1);

			setSelection(0, 0);
			return;

		case MKEY_DOWN:
			if(! isSingleLine())
				upCharId(1);

			setSelection(0, 0);
			return;

		case MKEY_RIGHT:
			addCharId();
			setSelection(0, 0);
			return;

		case MKEY_LEFT:
			subCharId();
			setSelection(0, 0);
			return;

		case MKEY_SPACE:
			if(selection)
			{
				m_text.erase(m_text.begin() + sStart, m_text.begin() + sEnd);
				setSelection(0, 0);
			}

			if(! canAddCharacter())
				return;

			m_text.insert(getCharId(), " ", 1);
			addCharId();
			autoScaleFromText();
			onChange();
			return;

		case MKEY_TAB:
			if(! isSingleLine())
			{
				if(selection)
				{
					m_text.erase(m_text.begin() + sStart, m_text.begin() + sEnd);
					setSelection(0, 0);
				}

				if(! canAddCharacter())
					return;

				m_text.insert(getCharId(), "	", 1);
				addCharId();
				autoScaleFromText();
				onChange();
			}
			return;

		case MKEY_BACKSPACE:
			if(selection)
			{
				m_text.erase(m_text.begin() + sStart, m_text.begin() + sEnd);
				setSelection(0, 0);
				autoScaleFromText();
				onChange();
			}
			else if(getCharId() > 0)
			{
				m_text.erase(m_text.begin() + getCharId() - 1);
				subCharId();
				autoScaleFromText();
				onChange();
			}
			return;

		case MKEY_DELETE:
			if(getCharId() < m_text.size())
			{
				if(selection)
				{
					m_text.erase(m_text.begin() + sStart, m_text.begin() + sEnd);
					setSelection(0, 0);
				}
				else
				{
					m_text.erase(m_text.begin() + getCharId());
				}

				autoScaleFromText();
				onChange();
			}
			return;

		case MKEY_RETURN:
			if(! isSingleLine())
			{
				if(selection)
				{
					m_text.erase(m_text.begin() + sStart, m_text.begin() + sEnd);
					setSelection(0, 0);
				}

				if(! canAddCharacter())
					return;

				m_text.insert(getCharId(), "\n", 1);
				addCharId();
				autoScaleFromText();
				onChange();
			}
			else
			{
				setHighLight(false);
				autoScaleFromText();
				setSelection(0, 0);
				sendVariable();
			}
			return;
		}
	}

	if(windowEvent->type == MWIN_EVENT_CHAR)
	{
		char character = windowEvent->data[0];

		if(character >= 0 && character < 33)
			return;

		// add character
		if(character)
		{
			if(selection)
			{
				m_text.erase(m_text.begin() + sStart, m_text.begin() + sEnd);
				setSelection(0, 0);
			}

			if(! canAddCharacter())
				return;

			m_text.insert(getCharId(), &character, 1);
			addCharId();
			autoScaleFromText();
			onChange();
		}
	}
}

void MGuiEditText::onEvent(MWinEvent * windowEvent)
{
	MGuiWindow * parent = getParentWindow();

	MMouse * mouse = MMouse::getInstance();

	switch(windowEvent->type)
	{
	case MWIN_EVENT_MOUSE_WHEEL_MOVE:
	case MWIN_EVENT_MOUSE_MOVE:
		if(parent->isHighLight() && isMouseInside())
		{
			setHighLight(true);

			if(m_pointerEvent) // send mouse move gui event
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
		else
		{
			setHighLight(false);
		}

		if(isPressed() && mouse->isLeftButtonPushed())
		{
			m_endSelectionId = getFont()->findPointedCharacter(
				getText(),
				getPosition(),
				getTextSize(),
				getMouseLocalPosition()
			);

			autoScrolling();
		}
		break;
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		if(isHighLight())
		{
			if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
			{
				// unpress all edit text
				unsigned int i;
				unsigned int size = parent->getEditTextsNumber();
				for(i=0; i<size; i++)
					parent->getEditText(i)->setPressed(false);

				setPressed(true);

				setCharId(
					getFont()->findPointedCharacter(
						getText(),
						getPosition(),
						getTextSize(),
						getMouseLocalPosition()
					)
				);

				// start select
				setSelection(getCharId(), getCharId());
			}

			if(m_pointerEvent) // send mouse button down gui event
			{
				MGuiEvent guiEvent;

				guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_DOWN;
				guiEvent.data[0] = windowEvent->data[0];

				m_pointerEvent(this, &guiEvent);
			}
		}
		else
		{
			if(isPressed() && windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
			{
				setPressed(false);
				sendVariable();
			}
		}
		break;
	case MWIN_EVENT_CHAR:
	case MWIN_EVENT_KEY_DOWN:
		if(isPressed())
		{
			editText(windowEvent);
			autoScrolling();
		}
		break;
	}
}

void MGuiEditText::draw(void)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	if(! isVisible())
		return;

	if(! isPressed())
		updateFromVariable();

	// draw selection
	if(isPressed())
	{
		render->disableTexture();
		render->setColor4(MVector4(1, 0.1f, 0.1f, 0.3f));
		getFont()->drawSelection(getText(), getPosition(), getTextSize(), m_startSelectionId, m_endSelectionId);
	}

	// set text color
	if(isPressed()){
		render->setColor4(getPressedColor());
	}
	else if(isHighLight()){
		render->setColor4(getHighLightColor());
	}
	else{
		render->setColor4(getNormalColor());
	}

	// draw text
	render->enableTexture();
	getFont()->draw(getText(), getPosition(), getTextSize());

	if(isPressed() && (m_startSelectionId == m_endSelectionId)) // cursor
	{
		render->disableTexture();

		render->setColor4(MVector4(1, 0, 0, 0.5f));

		// position
		MVector2 position = getFont()->getCharacterPosition(
			getText(),
			getPosition(),
			getTextSize(),
			getCharId()
		);

		// scale
		MVector2 scale = MVector2(getTextSize() * 0.1f, getTextSize());

		float offset = (getTextSize() - (getTextSize()*getFont()->getSpace()))*0.5f;
		float px = (float)((int)(position.x + offset));
		float py = (float)((int)position.y);

		// draw
		render->disableNormalArray();
		render->disableTexCoordArray();
		render->disableColorArray();
		render->enableVertexArray();

		MVector2 vertices[4] = {
			MVector2(px, py),
			MVector2(px, py + scale.y),
			MVector2(px + scale.x, py),
			MVector2(px + scale.x, py + scale.y)
		};

		render->setVertexPointer(M_FLOAT, 2, vertices);
		render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
	}

	MGuiEvent guiEvent;
	guiEvent.type = MGUI_EVENT_DRAW;
	if(m_pointerEvent)
		m_pointerEvent(this, &guiEvent);
}
