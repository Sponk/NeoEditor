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
 * diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <CheckButton.h>
#include <Neo2DEngine.h>
#include <Render.h>
#include <Window/Mouse.h>

using namespace Neo2D;
using namespace Gui;

CheckButton::CheckButton(unsigned int x, unsigned int y, unsigned int size, const char* label)
	: Widget(x, y, size, size, label),
	  m_button(x, y, size, size, ""),
	  m_label(x + size + 3, y + size*0.5, 0, 0, label),
	  m_value(false)
{
	m_button.setCallback(CheckButton::buttonCallback);
	m_button.setUserData((long int) this);
}

void CheckButton::draw(Vector2 offset)
{
	Widget::draw(offset);
	m_button.draw(Vector2());
	m_label.draw();
}

void CheckButton::update()
{
	m_button.setOffset(m_offset);
	m_button.update();
	m_label.update();
}

void CheckButton::buttonCallback(Widget* w, long int data)
{
	CheckButton* b = (CheckButton*) data;
	b->setValue(!b->getValue());

	b->doCallback();
}