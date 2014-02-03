/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiMenu.cpp
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
MGuiMenu::MGuiMenu(const MVector2 & position, const MVector2 & scale, const MVector4 & color, void (* pointerEvent)(MGuiMenu * menu, MGuiEvent * guiEvent)):
m_menuId(0),
m_window(MVector2(), MVector2(), MVector4(), NULL)
{
	setParentWindow(NULL);
	setTextSize(16);
	setPosition(position);
	setScale(scale);
	setNormalColor(color);
	setHighLightColor(color);
	setPressedColor(color);
	setTextColor(MVector4(0, 0, 0, 0.5f));
	setPointerEvent(pointerEvent);
	
	m_window.setMargin(MVector2(0, 0));
}

MGuiMenu::~MGuiMenu(void)
{
	clearWindowMenu();
}

void MGuiMenu::setMenuId(unsigned int id)
{
	m_menuId = id;
	autoScaleFromText();
}

void MGuiMenu::sendVariable(void)
{
	if(getVariablePointer())
	{
		switch(getVariableType())
		{
			case M_VAR_INT:
			{
				int * value = (int *)getVariablePointer();
				*value = (int)getMenuId();
			}
				break;
			case M_VAR_UINT:
			{
				unsigned int * value = (unsigned int *)getVariablePointer();
				*value = getMenuId();
			}
				break;
		}
	}
	
	if(m_pointerEvent)
	{
		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_SEND_VARIABLE;
		m_pointerEvent(this, &guiEvent);
	}
}

void MGuiMenu::updateFromVariable(void)
{
	if(! getVariablePointer())
		return;
	
	switch(getVariableType())
	{
		case M_VAR_INT:
		{
			int * value = (int *)getVariablePointer();
			setMenuId(*value);
			autoScaleFromText();
		}
		case M_VAR_UINT:
		{
			unsigned int * value = (unsigned int *)getVariablePointer();
			setMenuId(*value);
			autoScaleFromText();
		}
	}
}

void MGuiMenu::updateSelection(void)
{
	unsigned int oldId = getMenuId();
	if(findMenuId())
	{
		sendVariable();
		
		if(getMenuId() != oldId)
			onChange();
	}
}

void MGuiMenu::setPressed(bool pressed)
{
	MGui2d::setPressed(pressed);
}

bool MGuiMenu::findMenuId(void)
{
	// find menu_id
	unsigned int i;
	unsigned int bSize = m_window.getButtonsNumber();
	for(i=0; i<bSize; i++){
		if(m_window.getButton(i)->isMouseInside())
		{
			m_menuId = i;
			return true;
		}
	}
	
	return false;
}

void MGuiMenu::onChange(void)
{
	// send on change gui event
	if(m_pointerEvent)
	{
		MGuiEvent guiEvent;
		guiEvent.type = MGUI_EVENT_ON_CHANGE;
		m_pointerEvent(this, &guiEvent);
	}
}

void MGuiMenu::onEvent(MWinEvent * windowEvent)
{
	MGuiWindow * parent = getParentWindow();
	
	MMouse * mouse = MMouse::getInstance();
	
	//event
	switch(windowEvent->type)
	{
		case MWIN_EVENT_MOUSE_WHEEL_MOVE:
		case MWIN_EVENT_MOUSE_MOVE:
			if((! isPressed()) && parent->isHighLight() && isMouseInside() && (! mouse->isLeftButtonPushed()) && (! mouse->isMiddleButtonPushed()) && (! mouse->isRightButtonPushed()))
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
			}
			else
			{
				setHighLight(false);
			}
			break;
		case MWIN_EVENT_MOUSE_BUTTON_DOWN: // mouse button down
			//if(isHighLight())
			if(parent->isHighLight() && isMouseInside())
			{
				if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT) // left mouse button
				{
					if(isPressed())
					{
						setPressed(false);
						autoScaleFromText();
					}
					else {
						setPressed(true);
						setHighLight(false);
						m_window.setHighLight(false);
						rescaleWindowMenu();
					}
				}
				
				if(m_pointerEvent) // send mouse button down gui event
				{
					MGuiEvent guiEvent;
					
					guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_DOWN;
					guiEvent.data[0] = windowEvent->data[0];
					
					m_pointerEvent(this, &guiEvent);
				}
			}
			else if(! m_window.isMouseInside())
			{
				if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT) // left mouse button
				{
					setPressed(false);
					autoScaleFromText();
				}
			}
			
			break;
		case MWIN_EVENT_MOUSE_BUTTON_UP:
			//if(isHighLight())
			if(parent->isHighLight() && isMouseInside())
			{
				if(m_pointerEvent) // send mouse button up gui event
				{
					MGuiEvent guiEvent;
					
					guiEvent.type = MGUI_EVENT_MOUSE_BUTTON_UP;
					guiEvent.data[0] = windowEvent->data[0];
					
					m_pointerEvent(this, &guiEvent);
				}
			}
			else if(isPressed() && (!isMouseInside()))
			{
				if(windowEvent->data[0] == MMOUSE_BUTTON_LEFT)
				{
					setPressed(false);
					if(m_window.isMouseInside())
						updateSelection();
					
					autoScaleFromText();
				}
			}
			break;
	}
}

const char * MGuiMenu::getText(void)
{
	unsigned int bSize = m_window.getButtonsNumber();
	
	if(m_menuId >= bSize)
		return NULL;
	
	return m_window.getButton(m_menuId)->getText();
}

void MGuiMenu::rescaleWindowMenu(void)
{
	MVector2 tmp;
	MVector2 newScale(0, 0);
	
	MGuiWindow * parent = getParentWindow();
	
	MWindow * window = MWindow::getInstance();
	
	// position
	MVector2 newPos = parent->getPosition() +
	parent->getScroll() +
	getPosition();
	
	// window scale
	unsigned int i;
	unsigned int bSize = m_window.getButtonsNumber();
	for(i=0; i<bSize; i++)
	{
		MGuiButton * button = m_window.getButton(i);
		tmp = button->getPosition()
		+ button->getScale();
		
		if(tmp.x > newScale.x) newScale.x = tmp.x;
		if(tmp.y > newScale.y) newScale.y = tmp.y;
	}
	
	unsigned int tSize = m_window.getTextsNumber();
	for(i=0; i<tSize; i++)
	{
		MGuiText * text = m_window.getText(i);
		tmp = text->getPosition()
		+ text->getScale();
		
		if(tmp.x > newScale.x) newScale.x = tmp.x;
		if(tmp.y > newScale.y) newScale.y = tmp.y;
	}
	
	// y
	if(newPos.y < window->getHeight()/2)
	{
		newPos.y += getScale().y;
		
		if((newPos.y + newScale.y) > window->getHeight()){
			newScale.y = window->getHeight() - newPos.y;
		}
	}
	else
	{
		newPos.y -= newScale.y;
		
		if(newPos.y < 0)
		{
			newScale.y += newPos.y;
			newPos.y = 0;
		}
	}
	
	// x
	if((newPos.x + newScale.x) > window->getWidth()){
		newPos.x -= ((newPos.x + newScale.x) - window->getWidth());
	}
	
	if(newPos.x < 0) newPos.x = 0;
	
	if((newPos.x + newScale.x) > window->getWidth()){
		newScale.x = window->getWidth() - newPos.x;
	}
	
	
	
	m_window.setPosition(newPos);
	m_window.setScale(newScale);
	
	// buttons size
	for(i=0; i<bSize; i++)
	{
		MGuiButton * button = m_window.getButton(i);
		tmp = button->getScale();
		
		if(newScale.x > tmp.x)
			tmp.x = newScale.x;
		
		button->setScale(tmp);
	}
	
	m_window.resizeScroll();
}

void MGuiMenu::drawWindowMenu(void)
{
	m_window.draw();
}

void MGuiMenu::addSimpleButton(const char * text, void (* buttonPointerEvent)(MGuiButton * button, MGuiEvent * guiEvent))
{
	float space = 4;
	MVector2 pos(0, 0);
	
	// position
	unsigned int i;
	unsigned int bSize = m_window.getButtonsNumber();
	
	for(i=0; i<bSize; i++){
		pos.y += m_window.getButton(i)->getScale().y + space;
	}
	
	// new button
	MGuiButton * btn = new MGuiButton(pos, MVector2(1, 1), MVector4(1, 1, 1, 0), buttonPointerEvent);
	
	btn->setText(text);
	btn->setTextSize(getTextSize());
	btn->setTextColor(getTextColor());
	btn->setPressedColor(getHighLightColor());
	btn->setHighLightColor(getHighLightColor());
	
	m_window.addButton(btn);
	
	autoScaleFromText();
}

void MGuiMenu::clearWindowMenu(void)
{
	m_window.clear();
}

void MGuiMenu::draw(void)
{
	if(! isVisible())
		return;
	
	if(! isPressed())
		updateFromVariable();
	
	MGui2d::draw();
	
	MGuiEvent guiEvent;
	guiEvent.type = MGUI_EVENT_DRAW;
	if(m_pointerEvent)
		m_pointerEvent(this, &guiEvent);
}
