/*
 * Copyright 2014 (C) Yannick Pflanzer <neo-engine.de>
 *
 * This file is part of NeoGui.
 *
 * NeoGui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NeoGui is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with NeoGui.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von NeoGui.
 *
 * NeoGui ist Freie Software: Sie können es unter den Bedingungen
 * der GNU Lesser General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * NeoGui wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU Lesser General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <Widget.h>

using namespace Neo::Gui;

Widget::Widget(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const char* label) :
m_x(x),
m_y(y),
m_rotation(0.0f),
m_width(width),
m_height(height),
m_label(label),
m_callback(NULL)
{

}

Widget::Widget():
m_x(0),
m_y(0),
m_rotation(0.0f),
m_width(30),
m_height(30),
m_label("Widget"),
m_callback(NULL)
{

}

void Widget::doCallback()
{
#ifdef WIN32
    ((void (*)(long int)) m_callback)(m_userData);
#else
    m_callback(m_userData);
#endif
}
