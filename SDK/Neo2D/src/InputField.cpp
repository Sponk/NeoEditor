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

#include <InputField.h>
#include <Render.h>
#include <Neo2DEngine.h>
#include <Window/Keyboard.h>
#include <Window/Mouse.h>
#include <Window/WinEvents.h>
#include <tinyutf8.h>

//#include <codecvt>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

InputField::InputField(unsigned int x, unsigned int y, unsigned int width,
					   unsigned int height, const char* label)
	: Widget(x, y, width, height, label),
	  m_labelText(NULL),
	  m_cursorpos(0)
{

}

float InputField::calculateWidth(OText* text)
{
	float length = 0;
	Font* font = text->getFontRef()->getFont();
	map<unsigned int, Character>* chars = font->getCharacters();

	unsigned int state = 0;
	unsigned int character;
	unsigned char* s = (unsigned char*) text->getText();
	while(*s)
	{
		if(utf8_decode(&state, &character, *s) == UTF8_ACCEPT)
			length += ((*chars)[character]).getXAdvance();

		s++;
	}
	
	return length * text->getSize();
}

void InputField::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();
	Neo2DEngine* gui = Neo2DEngine::getInstance();

	Widget::draw(offset);
	offset += getPosition();

	unsigned int winh, winw;
	system->getScreenSize(&winw, &winh);

	if (m_labelText == NULL)
	{
		m_labelText = render->createText(gui->getDefaultFont(),
										 m_fontSize);
		m_labelText->setAlign(TEXT_ALIGN_LEFT);
	}

	Vector4 color;
	switch (m_state)
	{
		case INPUT_NORMAL_STATE:
			color = 1.5 * Neo2DEngine::getInstance()->getNormalBackground();
			break;

		case INPUT_HOVER_STATE:
			color = 1.2 * Neo2DEngine::getInstance()->getHoverBackground();
			break;

		case INPUT_SELECTED_STATE:
			color = Neo2DEngine::getInstance()->getHighlightBackground();
			break;
	}

	// Update text
	updateLabel(m_label);

	Box3d* box = m_labelText->getBoundingBox();
	float textOffset = calculateWidth(m_labelText);

	// Calculate overflow offset
	if (textOffset <= m_width) textOffset = 0.0f; // If we do not overflow, simply ignore
	else textOffset -= m_width; // Calculate the new position relative to the right edge

	RenderingContext* renderContext =
		NeoEngine::getInstance()->getRenderingContext();

	render->drawColoredQuad(offset.x, offset.y, m_width, m_height, color);

	renderContext->enableScissorTest();
	renderContext->setScissor(offset.x, winh - (offset.y + m_height), m_width, m_height);

	render->drawText(m_labelText, offset.x - textOffset,
					offset.y + 0.5*(box->max.y - box->min.y) + 0.5 * m_height,
					 m_rotation);

	renderContext->disableScissorTest();
}

void InputField::updateLabel(string s)
{
	// Update text
	string prefix = s;
	prefix.erase(m_cursorpos);

	string postfix = s.substr(m_cursorpos);
	
	m_labelText->setText((prefix + ((m_state == INPUT_SELECTED_STATE) ? "|" : "") + postfix).c_str());
}

void InputField::update()
{
	if (m_labelText == nullptr) return;

#ifndef ANDROID
	MKeyboard* kbd = MKeyboard::getInstance();

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

	if (x >= wx && x <= wx + m_width && y >= wy && y <= wy + m_height &&
		m_state != INPUT_SELECTED_STATE)
	{
		m_state = INPUT_HOVER_STATE;
	}
	else if (m_state != INPUT_SELECTED_STATE)
	{
		m_state = INPUT_NORMAL_STATE;
	}
	else if (!(x >= wx && x <= wx + m_width && y >= wy &&
			   y <= wy + m_height) &&
			 m_state == INPUT_SELECTED_STATE && input->onKeyDown("MOUSE_BUTTON_LEFT"))
	{
		m_state = INPUT_NORMAL_STATE;
	}

	if (m_state == INPUT_HOVER_STATE && input->onKeyDown("MOUSE_BUTTON_LEFT"))
	{
		m_cursorpos = m_label.length();
		m_state = INPUT_SELECTED_STATE;
		return;
	}

	if (m_state == INPUT_SELECTED_STATE)
	{
		unsigned int c = input->popLastChar();

		if(input->isKeyPressed("LEFT") && m_cursorpos > 0)
			m_cursorpos--;

		if(input->isKeyPressed("RIGHT") && m_cursorpos < m_label.length())
			m_cursorpos++;
		
		if (input->isKeyPressed("BACKSPACE"))
		{
			input->upKey("BACKSPACE");
			if (m_label.length() >= 1 && m_cursorpos > 0)
			{
				auto endDelete = m_label.begin() + m_cursorpos;
				m_label.erase(endDelete - 1, endDelete);
				m_cursorpos--;
			}
		}

		if (input->isKeyPressed("DELETE"))
		{
			input->upKey("DELETE");
			if (m_label.length() >= 1 && m_cursorpos < m_label.length())
			{
				auto endDelete = m_label.begin() + m_cursorpos;
				m_label.erase(endDelete, endDelete + 1);
			}
		}

		if(input->isKeyPressed("ENTER") && c == '\0')
			c = '\n';

		input->upKey("LEFT");
		input->upKey("RIGHT");
		input->upKey("ENTER");

		if (c != '\0')
		{
			char character[5];
			string prefix = m_label;
			prefix.erase(m_cursorpos);
						
			string postfix = m_label.substr(m_cursorpos);
			
			m_label = prefix + (const char*) &c + postfix;
			m_cursorpos += strlen((const char*) &c);
		}
	}

#endif
}
