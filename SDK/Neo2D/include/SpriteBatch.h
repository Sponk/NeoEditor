/*
 * Copyright 2014-2015 (C) Yannick Pflanzer <neo-engine.de>
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

#ifndef NEO_SPRITEBATCH_H
#define NEO_SPRITEBATCH_H

#include <NeoCore.h>
#include <Widget.h>
#include <vector>

namespace Neo2D
{

// Forward declaration of Canvas
class Canvas;

/**
 * @brief Implements a batch of sprites that can be used to circumvent the
 * GUI system when rendering 2D objects for better performance.
 *
 * @par Creating a new SpriteBatch in C++
 * @code
 * Neo2DEngine* engine = Neo2DEngine::getInstance();
 *
 * // Fetch main canvas
 * Canvas* mainCanvas = engine->getCanvas(0);
 *
 * // Calling Sprite(x, y, w, h, file, label) with w = 0 and h = 0
 * // so the resolution of the texture is used
 * Sprite* sprite = new Sprite(100, 100, 0, 0, "assets/tex.png", "label");
 * SpriteBatch* batch = new SpriteBatch();
 *
 * // Add sprite do render pipeline
 * batch->addSprite(sprite);
 *
 * // Add to canvas
 * mainCanvas->addSpriteBatch(batch);
 *
 * @endcode
 *
 * @par Creating a new SpriteBatch in Lua
 * @code
 * require("NeoLua")
 * local engine = NeoLua.Neo2DEngine.getInstance()
 *
 * -- Fetch main canvas
 * local mainCanvas = gui:getCanvas(0);
 *
 * -- Calling Sprite(x, y, w, h, file, label) with w = 0 and h = 0
 * -- so the resolution of the texture is used
 * local sprite = NeoLua.Sprite(100, 100, 0, 0, "assets/tex.png", "label");
 * local batch = NeoLua.SpriteBatch()
 *
 * -- Add sprite do render pipeline
 * batch:addSprite(sprite);
 *
 * -- Add to canvas
 * mainCanvas:addSpriteBatch(batch);
 * @endcode
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT SpriteBatch
{
protected:
	std::vector<Widget *> m_sprites;
public:
	DISOWN(Widget* s) virtual void addSprite(Widget* s) { m_sprites.push_back(s); }
	virtual void draw(Vector2 offset = Vector2(0, 0));

	virtual unsigned int getNumSprites() { return m_sprites.size(); }
	virtual void deleteSprite(unsigned int id)
	{
		if(id >= m_sprites.size()) return;
		SAFE_DELETE(m_sprites[id]);
		m_sprites.erase(m_sprites.begin() + id);
	}

	virtual void update(Canvas* canvas) {};
};
}
#endif //NEO_SPRITEBATCH_H
