/*
 * Copyright 2015 (C) Yannick Pflanzer <neo-engine.de>
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

#ifndef __DRAGGABLE_ICON_H
#define __DRAGGABLE_ICON_H

#include <Sprite.h>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Models click modes for icons.
 */
enum ICON_CLICK_MODE
{
	SINGLE_CLICK, /// A single click
	DOUBLE_CLICK /// A double click
};

/**
 * @brief Implements an icon that is clickable with either a single or double click to execute a callback.
 *
 * Useful for file system viewers and browsers and similar applications.
 * @author Yannick Pflanzer
 */
class DraggableIcon: public Sprite
{
	bool m_selected;
	ICON_CLICK_MODE m_clickMode;

public:
	DraggableIcon(float x, float y, unsigned int w, unsigned int h, const char* filename, const char* title) :
		Sprite(x, y, w, h, filename, title),
		m_selected(false),
		m_clickMode(DOUBLE_CLICK)
	{}

	/**
	 * @return The click mode, i.e. single or double click.
	 */
	const ICON_CLICK_MODE &getClickMode() const
	{
		return m_clickMode;
	}

	/**
	 * @brief Sets the click mode to either single or double click.
	 * @param clickMode The new click mode.
	 */
	void setClickMode(const ICON_CLICK_MODE& clickMode)
	{
		this->m_clickMode = m_clickMode;
	}

	void update();
	void draw(Vector2 offset);
};
}
}

#endif
