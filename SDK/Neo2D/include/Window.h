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

#ifndef __WINDOW_H
#define __WINDOW_H

#include <Neo2DEngine.h>
#include <Container.h>
#include <WindowManager.h>
#include <vector>

#define TITLE_HEIGHT 30

namespace Neo2D
{
namespace Gui
{
using namespace Neo;

/**
* Represents the state a window can possibly have.
*/
enum WINDOW_STATE
{
	WINDOW_UNSELECTED_STATE = 0,
	WINDOW_GRABBED_STATE,
	WINDOW_SELECTED_STATE,
	WINDOW_RESIZING_STATE
};

/**
 * @brief This class implements a Window that can be used to display widgets.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Window : public Container
{

	protected:
		OText* m_labelText;

		float m_mx;
		float m_my;

		WINDOW_STATE m_state;
		WindowManager* m_wm;

		bool m_resizable;

public:
		Window(unsigned int x, unsigned int y, unsigned int width,
			   unsigned int height, const char* label)
			: Container(x, y, width, height, label),
			  m_labelText(NULL),
			  m_state(WINDOW_SELECTED_STATE),
			  m_wm(NULL),
			  m_resizable(true)

		{
		}

		~Window();

		void setWindowManager(WindowManager* wm) { m_wm = wm; }

		bool containsPoint(float x, float y)
		{
			return (x >= m_x && x <= m_x + m_width && y >= m_y-TITLE_HEIGHT &&
					y <= m_y + m_height);
		}

		void draw();
		void draw(Vector2 offset) { draw(); }
		void update();

		virtual bool isMouseOver();

		bool isResizable() const
		{
			return m_resizable;
		}

		void setResizable(bool resizable)
		{
			m_resizable = resizable;
		}
};

}
}

#endif
