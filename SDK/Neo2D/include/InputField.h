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

#ifndef __INPUT_H__
#define __INPUT_H__

#include <Widget.h>
#include <NeoEngine.h>
#include <string>

namespace Neo2D
{
namespace Gui
{

using namespace Neo;

enum INPUT_STATE
{
	/// The input field is neither hovered over nor selected
	INPUT_NORMAL_STATE = 0,
	/// The input field is hovered by the mouse cursor
	INPUT_HOVER_STATE,
	/// The input field currently has focus
	INPUT_SELECTED_STATE
};

enum INPUT_TYPE
{
	NORMAL_INPUT = 0,
	DECIMAL_INPUT,
	INTEGER_INPUT
};

/**
 * @brief Implements a basic input field that takes keyboard input and
 * delivers it to the program.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT InputField : public Widget
{
protected:
	OText* m_labelText;
	INPUT_STATE m_state;
	unsigned int m_cursorpos;
	
	INPUT_TYPE m_inputType;
	bool m_multiline;

	void updateLabel(string s);

	/**
	 * @brief Calculates the full width of the given text object.
	 * @param text The text object to operate on.
	 * @return The full width in pixels
	 */
	Vector2 calculateCursorPos(OText* text, int pos = -1);
	void addChar(unsigned int c);
	
public:
	InputField(unsigned int x, unsigned int y, unsigned int width,
			   unsigned int height, const char* label);

	void draw(Vector2 offset);
	void update();

	void setState(INPUT_STATE state) { m_state = state; }
	const char* getStaticName() { return "InputField"; }

	void setLabel(const char* str) { m_cursorpos = 0; Widget::setLabel(str); }

	INPUT_TYPE getType() { return m_inputType; }
	void setType(INPUT_TYPE t) { m_inputType = t; }

	bool isMultiline() { return m_multiline; }
	void setMultiline(bool b) { m_multiline = b; }
};
}
}

#endif
