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

#ifndef __SCROLLBAR_H__
#define __SCROLLBAR_H__

#include <string>
#include <NeoEngine.h>
#include <Widget.h>
#include <Slider.h>

namespace Neo2D
{
namespace Gui
{

using namespace Neo;

/**
 * @brief The ScrollBar class implements a slider that interpolates values
 * in a given range.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT ScrollBar : public Widget
{
	SLIDER_DIRECTION m_direction;
	SLIDER_STATE m_state;

	Vector2 m_range;
	float m_value;

	float m_mx;
	float m_my;

public:
	ScrollBar(unsigned int x, unsigned int y, unsigned int width,
		  unsigned int height, float min, float max, SLIDER_DIRECTION dir)
		: Widget(x, y, width, height, ""),
		  m_direction(dir),
		  m_range(min, max),
		  m_mx(0),
		  m_my(0),
		  m_value(0),
		  m_state(SLIDER_NORMAL)
	{}

	Vector2 getRange() { return m_range; }
	void setRange(Vector2 range) { m_range = range; }

	void setValue(float value) { m_value = value; }
	float getValue() { return m_value; }

	void draw(Vector2 offset);
	void update();
};
}
}
#endif
