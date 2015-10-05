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

#include <Widget.h>

using namespace Neo2D;

Widget::Widget(unsigned int x, unsigned int y, unsigned int width,
			   unsigned int height, const char* label)
	: m_x(x),
	  m_y(y),
	  m_rotation(0.0f),
	  m_width(width),
	  m_height(height),
	  m_label(label),
	  m_callback((NEO_CALLBACK_FUNCTION) Widget::doNothing),
	  m_visible(true),
	  m_scale(Vector2(1, 1)),
	  m_flip(Vector2(0, 180)),
	  m_fontSize(12.0),
	  m_userData(0),
	  m_parent(NULL)
{
}

Widget::Widget()
	: m_x(0),
	  m_y(0),
	  m_rotation(0.0f),
	  m_width(30),
	  m_height(30),
	  m_label("Widget"),
	  m_callback((NEO_CALLBACK_FUNCTION) Widget::doNothing),
	  m_visible(true),
	  m_scale(Vector2(1, 1)),
	  m_flip(Vector2(0, 180)),
	  m_fontSize(12.0),
	  m_userData(0),
	  m_parent(NULL)
{
}

void Widget::doCallback()
{
//#ifdef WIN32
//	((void (*)(Widget*, long int))m_callback)(this, m_userData);
//#else
	m_callback(this, m_userData);
//#endif
}

bool Widget::isMouseOver()
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

	return isVisible() && (x >= wx && x <= wx + m_width
			&& y >= wy && y <= wy + m_height);
}
