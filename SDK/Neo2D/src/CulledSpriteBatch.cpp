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
#include "CulledSpriteBatch.h"
#include <Neo2DEngine.h>
/**
 * local function isColliding(x1, y1, w1, h1, x2, y2, w2, h2)
  return x1 < x2+w2 and
         x2 < x1+w1 and
         y1 < y2+h2 and
         y2 < y1+h1
end
 */
void Neo2D::CulledSpriteBatch::updateVisibility(Neo2D::Canvas* viewport)
{
	// Get main viewport if argument is not set.
	viewport = (viewport == NULL) ? Neo2D::Neo2DEngine::getInstance()->getCanvas(0) : viewport;
	Vector2 campos = viewport->getCameraOffset();
	Vector2 size = viewport->getSize();

	Vector2 wpos;
	Vector2 wsize;
	for(Widget* w : this->m_sprites)
	{
		wsize = w->getSize();
		wpos = w->getPosition();

		// Check collision with viewport
		w->setVisible(wpos.x < campos.x + size.x
					  && campos.x < wpos.x + wsize.x
					  && wpos.y < campos.y + size.y
					  && campos.y < wpos.y + wsize.y);
	}
}
