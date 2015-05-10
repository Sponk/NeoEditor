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
class NEO2D_EXPORT Label : public Widget
{
protected:
	OText* m_labelText;
	TEXT_ALIGN_MODES m_alignment;
	String m_font;

public:
	Label(unsigned int x, unsigned int y, unsigned int width,
		  unsigned int height, const char* label)
		: Widget(x, y, width, height, label), m_labelText(NULL), m_alignment(TEXT_ALIGN_LEFT), m_font("assets/default.ttf")
	{
	}

	/**
	 * @brief Changes the text alignment.
	 * 
	 * Defaults to TEXT_ALIGN_LEFT.
	 * @param mode The align mode.
	 */
	void setAlignment(int mode) { m_alignment = (Neo::TEXT_ALIGN_MODES) mode; }

	/**
	 * @brief Returns the current align mode.
	 * @return The align mode.
	 * @see setAlignment
	 */
	TEXT_ALIGN_MODES getAlignment() { return m_alignment; }

	/**
	 * @brief Changes the font used to display text.
	 *
	 * The font file will be loaded in the first draw call and thus can't be
	 * changed for now. Defaults to "assets/default.ttf".
	 * @param file The font file to load.
	 */
	void setFont(const char* file) { m_font = file; }
	
	/**
	 * @brief Returns the path to the currently used font file.
	 * @return The font file.
	 * @see setFont
	 */
	const char* getFont() { return m_font.getSafeString(); }

	void draw();
	void draw(Vector2 offset) { draw(); }
	void update();
};
}
}
#endif
