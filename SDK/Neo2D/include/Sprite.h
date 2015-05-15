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
 * diesem Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <string>
#include <NeoEngine.h>
#include <Widget.h>

namespace Neo2D
{
	
using namespace Neo;
	
/**
 * @brief The Sprite class displays a texture on the screen.
 *
 * @warning This way of using Sprites is very slow since the GUI system is not
 * designed for displaying 2D graphics but displaying reusable GUI data!
 * Use a SpriteBatch to display large numbers of Sprite's or Tile's instead! 
 *
 * @par Creating a new Sprite in C++
 * @code
 * Neo2DEngine* engine = Neo2DEngine::getInstance();
 *
 * // Fetch main canvas
 * Canvas* mainCanvas = engine->getCanvas(0);
 * 
 * // Calling Sprite(x, y, w, h, file, label) with w = 0 and h = 0
 * // so the resolution of the texture is used 
 * Sprite* sprite = new Sprite(100, 100, 0, 0, "assets/tex.png", "label");
 *
 * // Register with the system
 * int handle = engine->addWidget(sprite);
 *
 * // Add to canvas
 * mainCanvas->addWidget(handle);
 *
 * @endcode
 *
 * @par Creating a new Sprite in Lua
 * @code
 * require("NeoLua")
 * local engine = NeoLua.Neo2DEngine.getInstance()
 *
 * -- Fetch main canvas
 * local mainCanvas = engine:getCanvas(0);
 * 
 * -- Calling Sprite(x, y, w, h, file, label) with w = 0 and h = 0
 * -- so the resolution of the texture is used 
 * local sprite = NeoLua.Sprite(100, 100, 0, 0, "assets/tex.png", "label");
 *
 * -- Register with the system
 * local handle = engine:addWidget(sprite);
 *
 * -- Add to canvas
 * mainCanvas:addWidget(handle);
 * @endcode
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Sprite : public Widget
{
protected:
	OText *m_labelText;
	int m_image;
	Vector2 m_imageSize;

	std::string m_imagePath;

	/**
	 * @brief Loads the texture into memory and registers it with OpenGL.
	 */
	void loadTexture();

public:
	/**
	 * @brief Creates a new Sprite with the given options.
	 *
	 * @param x The X position.
	 * @param y The Y position.
	 * @param width The width. If this value is 0 it will take the 
	 * width of the texture
	 *
	 * @param height The height. If this value is 0 it will take the height of the
	 * texture.
	 *
	 * @param texture The path of the texture to use and display.
	 * @param label The label to display.
	 */
	Sprite(unsigned int x, unsigned int y, unsigned int width,
		   unsigned int height, const char *texture, const char *label);

	void draw(Vector2 offset);
	void update();

	/**
	 * @brief Returns the textures dimensions that are used.
	 * @return The dimensions of the texture in pixels.
	 */
	Vector2 getSize() { return m_imageSize; }
};
}
#endif
