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

#ifndef __RENDER_H__
#define __RENDER_H__

#include <NeoEngine.h>
#include <Neo2D.h>

namespace Neo2D
{

using namespace Neo;

/**
 * @brief The Render class contains helper methods for rendering 2D shapes.
 *
 * All elements can be rendered directly to the screen or to a texture.
 *
 * @bug Should be implemented in Neo::RenderingContext!
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Render
{
private:
	unsigned int m_colorOnlyFx;
	unsigned int m_texturedFx;
	int m_right, m_left, m_top, m_bottom;

public:
	static Render* getInstance()
	{
		static Render m_instance;
		return &m_instance;
	}

	/**
	 * @brief Draws a colored quad without texture to the screen.
	 *
	 * @param x The X-Position
	 * @param y The Y-Position
	 * @param w The width
	 * @param h The height
	 * @param color The color of the quad in RGBA between 0.0 and 1.0
	 * @param rotation The rotation of the quad.
	 */
	void drawColoredQuad(float x, float y, float w, float h, Vector4 color,
						 float rotation = 0);
	/**
	 * @brief Draws a textured quad to the screen.
	 *
	 * @param x The X-Position
	 * @param y The Y-Position
	 * @param w The width
	 * @param h The height
	 * @param texture The handle of the texture to use
	 * @param rotation The rotation of the quad.
	 */
	void drawTexturedQuad(float x, float y, float w, float h, int texture,
						  float rotation = 0);
	/**
	 * @brief Draws a textured quad to the screen.
	 *
	 * @param x The X-Position
	 * @param y The Y-Position
	 * @param w The width
	 * @param h The height
	 * @param texture The handle of the texture to use
	 * @param rotation The rotation of the quad
	 * @param scale The scale of the quad
	 */
	void drawTexturedQuad(float x, float y, float w, float h, int texture,
						  float rotation, Vector2 scale);

	/**
	 * @brief Draws a textured quad to the screen.
	 *
	 * @param x The X-Position
	 * @param y The Y-Position
	 * @param w The width
	 * @param h The height
	 * @param texture The handle of the texture to use
	 * @param rotation The rotation of the quad
	 * @param scale The scale of the quad
	 * @param flip The vector that specifies how to flip the quad.
	 */
	void drawTexturedQuad(float x, float y, float w, float h, int texture,
						  float rotation, Vector2 scale, Vector2 flip);

	/**
	 * @brief Draws a textured quad to the screen.
	 *
	 * @param x The X-Position
	 * @param y The Y-Position
	 * @param w The width
	 * @param h The height
	 * @param texture The handle of the texture to use
	 * @param rotation The rotation of the quad
	 * @param scale The scale of the quad
	 * @param flip The vector that specifies how to flip the quad.
	 * @param texcoords Custom texture coordinates
	 */
	void drawTexturedQuad(float x, float y, float w, float h, int texture,
						  float rotation, Vector2 scale, Vector2 flip,
						  Vector4 texcoords);

	/**
	 * @brief Draws a text object to the screen in 2D.
	 *
	 * @param text The OText object to draw.
	 * @param x The X-Position
	 * @param y The Y-Position
	 * @param rotation The rotation of the text
	 */ 
	void drawText(OText* text, float x, float y, float rotation = 0);

	/**
	 * @brief Sets a 2D viewport with the given width and height.
	 *
	 * @param w The width of the viewport.
	 * @param h The height of the viewport.
	 */
	void set2D(float w, float h);

	/**
	 * @brief Loads, compiles and links a GLSL shader.
	 *
	 * The shader ID will be plased inside the integer specified by the
	 * fx pointer.
	 *
	 * @param vert The vertex shader.
	 * @param frag The fragment shader.
	 * @param fx A pointer to and existing integer.
	 */
	void loadShader(const char* vert, const char* frag, unsigned int* fx);

	/**
	 * @brief Creates a new text object.
	 *
	 * @param font The font file to use.
	 * @param size The font size to use.
	 * @return The new OText object.
	 */
	OText* createText(const char* font, float size);

	Render();
};
}
#endif
