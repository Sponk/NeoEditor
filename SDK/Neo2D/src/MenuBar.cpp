/*
 * Copyright 2014 (C) Yannick Pflanzer <neo-engine.de>
 *
 * This file is part of Neo2D.
 *
 * Neo2D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Neo2D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Neo2D.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von Neo2D.
 *
 * Neo2D ist Freie Software: Sie können es unter den Bedingungen
 * der GNU Lesser General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * Neo2D wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU Lesser General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <MenuBar.h>
#include <Render.h>
#include <Neo2DEngine.h>

using namespace Neo2D;
using namespace Gui;

#define MENU_WIDTH 70

MenuBar::~MenuBar()
{
	for (Button b : m_buttons)
	{
		CallbackData* cdata = (CallbackData*)b.getUserData();
		SAFE_DELETE(cdata);
	}
}

void MenuBar::update()
{	
	for (Menu* l : m_entries)
	{
		if(l->isVisible())
			l->update();
	}

	for(int i = 0; i < m_buttons.size(); i++)
		m_buttons[i].update();
}

void MenuBar::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	Vector2 res = NeoEngine::getInstance()->getSystemContext()->getScreenSize();

	m_width = res.x;
	render->drawColoredQuad(0,0,res.x, m_height, Vector3(0.3,0.3,0.3));
	
	for (Menu* l : m_entries)
	{
		if(l->isVisible())
			l->draw(offset);
	}

	for(Button b : m_buttons)
		b.draw(offset);
}

void MenuBar::addEntry(Menu* l)
{
	float x = m_entries.size() * MENU_WIDTH;
	l->setPosition(Vector2(x, m_height));

	m_entries.push_back(l);

	Button b(x, 0, MENU_WIDTH, m_height, l->getLabel());
	CallbackData* data = new CallbackData();
	data->index = m_buttons.size();
	data->parent = this;
	
	b.setCallback(MenuBar::buttonCallback);
	b.setUserData((long int) data);
	
	m_buttons.push_back(b);
}

void MenuBar::buttonCallback(Widget* w, long int data)
{
	CallbackData* cdata = (CallbackData*) data;
	cdata->parent->getEntry(cdata->index)->setVisible(true);
}

bool MenuBar::isMouseOver()
{
	if(Widget::isMouseOver()) return true;
	for(Menu* m : m_entries)
	{
		m->setOffset(m_offset);
		if(m->isVisible() && m->isMouseOver())
		{
			return true;
		}
	}

	return false;
}