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

#include <Button.h>
#include <Neo2DEngine.h>
#include <Render.h>
#include <Window/Mouse.h>

using namespace Neo2D;
using namespace Gui;

Button::Button(unsigned int x, unsigned int y, unsigned int width,
			   unsigned int height, const char* label)
	: Widget(x, y, width, height, label),
	  m_labelText(NULL),
	  m_state(BUTTON_NORMAL_STATE)
{
}

void Button::draw()
{
	Render* render = Render::getInstance();
	Neo2DEngine* gui = Neo2DEngine::getInstance();

	if (m_labelText == NULL)
	{
		m_labelText = render->createText(gui->getDefaultFont(),
										 gui->getDefaultFontSize());
		m_labelText->setAlign(TEXT_ALIGN_CENTER);
	}

	m_labelText->setText(m_label.c_str());

	Vector4 color;
	switch (m_state)
	{
		case BUTTON_NORMAL_STATE:
			color = Neo2DEngine::getInstance()->getNormalBackground();
			break;

		case BUTTON_HOVER_STATE:
			color = Neo2DEngine::getInstance()->getHoverBackground();
			break;

		case BUTTON_PRESSED_STATE:
			color = Neo2DEngine::getInstance()->getHighlightBackground();
			break;
	}

	render->drawColoredQuad(m_x, m_y, m_width, m_height, color, m_rotation);
	render->drawText(m_labelText, m_x + 0.5 * static_cast<float>(m_width),
					 m_y + 0.5 * m_labelText->getSize() +
						 0.5 * static_cast<float>(m_height),
					 m_rotation);
}

void Button::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	unsigned int x = 0;
	unsigned int y = 0;
	Vector2 res = system->getScreenSize();
	x = input->getAxis("MOUSE_X") * res.x;
	y = input->getAxis("MOUSE_Y") * res.y;

	if (m_state == BUTTON_PRESSED_STATE && !input->isKeyPressed("MOUSE_BUTTON_LEFT"))
	{
		doCallback();
	}

	if (x >= m_x && x <= m_x + m_width && y >= m_y && y <= m_y + m_height)
	{
		m_state = BUTTON_HOVER_STATE;
	}
	else
	{
		m_state = BUTTON_NORMAL_STATE;
	}

	if (m_state == BUTTON_HOVER_STATE && input->isKeyPressed("MOUSE_BUTTON_LEFT"))
	{
		m_state = BUTTON_PRESSED_STATE;
	}
}
