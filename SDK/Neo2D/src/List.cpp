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
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit
 *diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <List.h>
#include <Render.h>
#include <Neo2DEngine.h>
#include <ThemedButton.h>

#define LINE_HEIGHT 25

using namespace Neo2D;
using namespace Gui;

void List::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	InputContext* input = engine->getInputContext();

	for (int i = 0; i < m_entries.size(); i++)
	{
		m_entries[i]->setPosition(Vector2(m_x, m_y + LINE_HEIGHT * i));
		m_entries[i]->setOffset(Vector2(m_offset.x,m_offset.y));
		m_entries[i]->update();

		if(i == m_selectedEntry)
			m_entries[i]->setButtonState(BUTTON_HOVER_STATE);
	}
}

void List::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	Neo2DEngine* gui = Neo2DEngine::getInstance();

	m_offset = offset + getPosition();
	render->drawColoredQuad(m_offset.x, m_offset.y, m_width, m_height, Vector4(1,1,1,1));

	unsigned int i = 0;
	unsigned int line = 0;
	for (Button* l : m_entries)
	{
		/*if(i == m_selectedEntry)
			render->drawColoredQuad(m_x, m_y - line, m_width, LINE_HEIGHT, Vector3(0.75,0.75,0.75));
		else
			render->drawColoredQuad(m_x, m_y - line, m_width, LINE_HEIGHT, Vector3(0.5,0.5,0.5));*/

		l->setPosition(Vector2(m_offset.x, m_offset.y + line));
		l->draw(Vector2(/*m_x, m_y - line*/ 0,0));
		line += LINE_HEIGHT;
		i++;
	}
}

void List::addEntry(Button* l)
{
	l->setSize(m_width, LINE_HEIGHT);
	l->setAlignment(TEXT_ALIGN_LEFT);

	m_entries.push_back(l);
}

void List::addEntry(const char* label)
{
	Button* btn = new Button(0,0,0,0, label);
	btn->setCallback(listCallback);

	CallbackData* data = new List::CallbackData();
	data->idx = m_entries.size();
	data->list = this;

	btn->setUserData((long int) data);
	addEntry(btn);
}


void List::listCallback(Widget* w, long int data)
{
	CallbackData* cd = (CallbackData*) data;
	cd->list->setSelectedEntry(cd->idx);
	cd->list->doCallback();
}
