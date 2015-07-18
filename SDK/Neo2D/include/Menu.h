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

#ifndef __MENU_H__
#define __MENU_H__

#include <string>
#include <vector>
#include <NeoEngine.h>
#include <Widget.h>
#include <Button.h>

#define LINE_HEIGHT 20

namespace Neo2D
{
namespace Gui
{

using namespace Neo;
	
/**
 * @brief The Menu class implements a basic drop-down menu.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Menu : public Widget
{
protected:

	bool m_waitingForInit;
	int m_selectedEntry;
	std::vector<Button> m_entries;
	String m_font;

public:
	Menu(unsigned int x, unsigned int y, unsigned int width,
		  unsigned int height, const char* label)
		: Widget(x, y, width, height, label), m_font("assets/default.ttf"),
		m_selectedEntry(-1), m_waitingForInit(false)
	{
		setVisible(false);
	}

	Menu(unsigned int x, unsigned int y)
		: Menu(x, y, 200, 0, "")
	{
	}

	void show() { m_visible = true; m_waitingForInit = true; }
	Button* getEntry(unsigned int idx) { return &m_entries[idx]; }
	void addEntry(Button l);

	void draw(Vector2 offset);
	void update();
};
}
}
#endif
