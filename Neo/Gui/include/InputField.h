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

#ifndef __INPUT_H__
#define __INPUT_H__

#include <Widget.h>
#include <NeoEngine.h>

namespace Neo
{
namespace Gui
{
enum INPUT_STATE
{
	/// The input field is neither hovered over nor selected
	INPUT_NORMAL_STATE = 0,
	/// The input field is hovered by the mouse cursor
	INPUT_HOVER_STATE,
	/// The input field currently has focus
	INPUT_SELECTED_STATE
};

/**
 * @brief Implements a basic input field that takes keyboard input and
 * delivers it to the program.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT InputField : public Widget
{
	OText* m_labelText;
	INPUT_STATE m_state;

	void addCharacter(char c);

	/**
	 * @brief Calculates the full width of the given text object.
	 * @param text The text object to operate on.
	 * @return The full width in pixels
	 */
	float calculateWidth(OText* text);
	
public:
	InputField(unsigned int x, unsigned int y, unsigned int width,
			   unsigned int height, const char* label);

	void draw();
	void draw(Vector2 offset) { draw(); }
	void update();
};
}
}

#endif
