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
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit
 *diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#ifndef __LABEL_H__
#define __LABEL_H__

#include <string>
#include <NeoEngine.h>
#include <Widget.h>

namespace Neo
{
namespace Gui
{
/**
 * @brief The Label class displays a string on the screen.
 *
 * @author Yannick Pflanzer
 */
class Label : public Widget
{
protected:
	OText* m_labelText;

public:
	Label(unsigned int x, unsigned int y, unsigned int width,
		  unsigned int height, const char* label)
		: Widget(x, y, width, height, label), m_labelText(NULL)
	{
	}

	void draw();
	void draw(MVector2 offset) { draw(); }
	void update();
};
}
}
#endif
