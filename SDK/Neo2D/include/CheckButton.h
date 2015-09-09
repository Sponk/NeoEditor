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

#ifndef __CHECKBUTTON_H__
#define __CHECKBUTTON_H__

#include <NeoEngine.h>
#include <Widget.h>
#include <Button.h>
#include <Label.h>

namespace Neo2D
{
namespace Gui
{

using namespace Neo;
 
/**
 * Represents the state a button can possibly have.
 */
enum CHECKBUTTON_STATE
{
	/// If the button is neither selected nor pressed
	CHECKBUTTON_NORMAL_STATE = 0,
	/// If the button has the mouse cursor over it
	CHECKBUTTON_HOVER_STATE,
	/// If the mouse is hovering over the button and the left mouse button is pressed
	CHECKBUTTON_PRESSED_STATE
};

/**
 * @brief Implements a simple check button that calls the specified callback and represents a boolean state.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT CheckButton : public Widget
{
protected:
	Button m_button;
	Label m_label;

	bool m_value;

	static void buttonCallback(Widget* w, long int data);

public:
	CheckButton(unsigned int x, unsigned int y, unsigned int size, const char* label);

	void setValue(bool v)
	{
		if(v) m_button.setLabel("x");
		else m_button.setLabel("");

		m_value = v;
	}

	bool getValue() { return m_value; }

	void setLabel(const char* label) { m_label.setLabel(label); }
	const char* getLabel() { return m_label.getLabel(); }

	void draw(Vector2 offset);
	void update();
};
}
}
#endif
