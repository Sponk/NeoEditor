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
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#ifndef __MENUBAR_H__
#define __MENUBAR_H__

#include <string>
#include <vector>
#include <NeoEngine.h>
#include <Widget.h>
#include <Button.h>
#include <Menu.h>

namespace Neo2D
{
namespace Gui
{

	static int MENU_LINE_HEIGHT = 20;

using namespace Neo;
	
/**
 * @brief The MenuBar class implements the global menu bar on the top of the window.
 * 
 * @todo Can't be used for Neo2D windows because it uses the main window size for its width.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT MenuBar : public Widget
{
protected:

	int m_selectedEntry;
	std::vector<Menu*> m_entries;
	std::vector<Button> m_buttons;

	static void buttonCallback(Widget* w, long int data);

	struct CallbackData
	{
		unsigned int index;
		MenuBar* parent;
	};
	
public:
	MenuBar(unsigned int height)
		: Widget(0, 0, 0, height, "")
	{
	}

	~MenuBar();
	
	Menu* getEntry(unsigned int idx) { return m_entries[idx]; }
	DISOWN(Menu* l) void addEntry(Menu* l);
	
	void draw(Vector2 offset);
	void update();

	virtual bool isMouseOver();
};
}
}
#endif
