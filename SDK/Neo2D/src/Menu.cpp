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

#include <Menu.h>
#include <Render.h>
#include <Neo2DEngine.h>

using namespace Neo2D;
using namespace Gui;

void Menu::update()
{	
	NeoEngine* engine = NeoEngine::getInstance();
	InputContext* input = engine->getInputContext();

	if (m_waitingForInit && input->onKeyUp("MOUSE_BUTTON_LEFT"))
	{
		m_waitingForInit = false;
		return;
	}
	
	if(m_waitingForInit) return;
	
	for (int i = 0; i < m_entries.size(); i++)
	{
		m_entries[i].setPosition(Vector2(m_x, m_y + LINE_HEIGHT * i));
		m_entries[i].update();
	}

	if(input->onKeyUp("MOUSE_BUTTON_LEFT"))
	{
		m_visible = false;
	}
}

void Menu::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	Neo2DEngine* gui = Neo2DEngine::getInstance();

	unsigned int i = 0;
	unsigned int line = 0;	
	for (Button l : m_entries)
	{
		/*if(i == m_selectedEntry)
			render->drawColoredQuad(m_x, m_y - line, m_width, LINE_HEIGHT, Vector3(0.75,0.75,0.75));
		else
			render->drawColoredQuad(m_x, m_y - line, m_width, LINE_HEIGHT, Vector3(0.5,0.5,0.5));*/

		l.setPosition(Vector2(m_x, m_y + line));
		l.draw(Vector2(/*m_x, m_y - line*/ 0,0));
		line += LINE_HEIGHT;
		i++;
	}
}

void Menu::addEntry(Button l)
{
	l.setSize(m_width, LINE_HEIGHT);
	l.setAlignment(TEXT_ALIGN_LEFT);
	
	m_entries.push_back(l);
}
