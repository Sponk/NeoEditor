/*
 * Copyright 2015 (C) Yannick Pflanzer <neo-engine.de>
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

#include <DraggableIcon.h>
#include <Render.h>

using namespace Neo2D;
using namespace Gui;

void DraggableIcon::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	unsigned int x = 0;
	unsigned int y = 0;
	Vector2 res = system->getScreenSize();
	x = input->getAxis("MOUSE_X") * res.x;
	y = input->getAxis("MOUSE_Y") * res.y;

	unsigned int wx = m_offset.x + m_x;
	unsigned int wy = m_offset.y + m_y;

	if (input->onKeyUp("MOUSE_BUTTON_LEFT")
		&& x >= wx && x <= wx + m_width
		&& y >= wy && y <= wy + m_height)
	{
		if(m_selected || m_clickMode == SINGLE_CLICK)
			doCallback();

		m_selected = true;
	}
	else if(input->onKeyUp("MOUSE_BUTTON_LEFT"))
		m_selected = false;
}

void DraggableIcon::draw(Vector2 offset)
{
	Render* render = Render::getInstance();

	Sprite::draw(offset);

	Vector2 sz = getSize();

	if(m_selected)
		render->drawColoredQuad(m_x + offset.x, m_y + offset.y, sz.x, sz.y + 5, Vector4(0,0,1,0.2));
}