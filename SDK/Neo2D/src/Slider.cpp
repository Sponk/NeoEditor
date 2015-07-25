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

#include <Slider.h>
#include <Render.h>
#include <Neo2DEngine.h>

using namespace Neo2D;
using namespace Gui;

#define LINE_HEIGHT 5
#define KNOB_HEIGHT 10
#define KNOB_WIDTH 15

void Slider::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	unsigned int x = 0;
	unsigned int y = 0;
	Vector2 res = system->getScreenSize();
	x = input->getAxis("MOUSE_X") * res.x;
	y = input->getAxis("MOUSE_Y") * res.y;

	float dx = x - m_mx;
	float dy = y - m_my;
	float delta = dx;
	float length = m_width;

	if(m_direction == SLIDER_VERTICAL)
	{
		length = m_height;
		delta = dy;
	}

	m_mx = x;
	m_my = y;

	Vector2 offset = m_offset + getPosition();
	Vector2 knobpos;
	Vector2 knobsize;

	if(m_direction == SLIDER_HORIZONTAL)
	{
		knobpos = Vector2(offset.x + (m_value/m_range.y)*m_width - KNOB_WIDTH*0.5, offset.y);
		knobsize = Vector2(KNOB_WIDTH, m_height * KNOB_HEIGHT);
	}
	else
	{
		knobpos = Vector2(offset.x, offset.y + (m_value/m_range.y) * m_height-KNOB_WIDTH*0.5);
		knobsize = Vector2(m_width * KNOB_HEIGHT, KNOB_WIDTH);
	}

	if(x >= knobpos.x && x <= knobpos.x + knobsize.x
			&& y >= knobpos.y && y <= knobpos.y + knobsize.y
			&& input->onKeyDown("MOUSE_BUTTON_LEFT")
			|| m_state == SLIDER_GRABBED)
	{
		m_state = SLIDER_GRABBED;
		m_value = MAX(0, MIN(m_range.y, (m_range.y/length)*delta + m_value));
		doCallback();
	}

	if(input->onKeyUp("MOUSE_BUTTON_LEFT"))
		m_state = SLIDER_NORMAL;
}

void Slider::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	Neo2DEngine* gui = Neo2DEngine::getInstance();

	m_offset = offset;
	offset += getPosition();

	switch(m_direction)
	{
	case SLIDER_HORIZONTAL:
			render->drawColoredQuad(offset.x, offset.y + KNOB_HEIGHT*0.25, m_width, m_height * LINE_HEIGHT, Vector4(1,1,1,1));
			render->drawColoredQuad(offset.x + (m_value/m_range.y) * m_width - KNOB_WIDTH*0.5,
									offset.y, KNOB_WIDTH, m_height * KNOB_HEIGHT, Vector4(0.5,0.5,0.5,1));
		break;

	case SLIDER_VERTICAL:
			render->drawColoredQuad(offset.x + KNOB_HEIGHT*0.25, offset.y, m_width * LINE_HEIGHT, m_height, Vector4(1,1,1,1));
			render->drawColoredQuad(offset.x, offset.y + (m_value/m_range.y) * m_height - KNOB_WIDTH*0.5,
									m_width * KNOB_HEIGHT, KNOB_WIDTH, Vector4(0.5,0.5,0.5,1));
		break;
	}
}
