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
#ifndef __CANVAS_H_
#define __CANVAS_H_

#include <Widget.h>
#include <Sprite.h>
#include <vector>
#include <NeoEngine.h>

namespace Neo
{
namespace Gui
{

/**
 * @brief Implements a batch of sprites that can be used to circumvent the
 * GUI system when rendering 2D objects for better performance.
 *
 * @par Creating a new SpriteBatch in C++
 * @code
 * GuiSystem* gui = GuiSystem::getInstance();
 *
 * // Fetch main canvas
 * Canvas* mainCanvas = gui->getCanvas(0);
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
 * require("NeoGui")
 * local gui = NeoGui.GuiSystem.getInstance()
 *
 * -- Fetch main canvas
 * local mainCanvas = gui:getCanvas(0);
 * 
 * -- Calling Sprite(x, y, w, h, file, label) with w = 0 and h = 0
 * -- so the resolution of the texture is used 
 * local sprite = NeoGui.Sprite(100, 100, 0, 0, "assets/tex.png", "label");
 * local batch = NeoGui.SpriteBatch()
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
	std::vector<Widget*> m_sprites;
public:
	DISOWN(Widget* s) void addSprite(Widget* s) { m_sprites.push_back(s); }
	void draw();
};
	
/**
 * @brief The Canvas class contains all widgets and renders them.
 *
 * A canvas contains all widgets, layouts, groups.
 * All elements can be rendered directly to the screen or to a texture.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Canvas
{
private:
	std::vector<int> m_widgets;
	Vector4 m_clearColor;

	unsigned int m_width, m_height;
	unsigned int m_fbo;
	bool m_renderToTexture;

	TextureRef* m_texture;
	int m_layer;

	Vector2 m_cameraPosition;

	std::vector<SpriteBatch*> m_batches;
public:
	/**
	 * @brief Returns the global canvas object that directly maps to the screen.
	 * @return The Canavs object.
	 */
	static Canvas* getInstance()
	{
		static Canvas m_instance;
		return &m_instance;
	}

	~Canvas() { clear(); }
	Canvas() : m_layer(0), m_renderToTexture(false), m_fbo(0), m_texture(NULL), m_width(0), m_height(0) {}

	DISOWN(SpriteBatch* b) void addSpriteBatch(SpriteBatch* b) { m_batches.push_back(b); }
	
	/**
	 * @brief Draws the Canvas and its contents.
	 */
	void draw();

	/**
	 * @brief Updates the Canvas and all its children.
	 */
	void update();

	/**
  	 * @brief Clears the Canvas by removing all children.
	 *
	 * Attention: No objects are deleted from memory!
	 */
	void clear();

	/**
	 * @brief Returns which layer this particular Canvas is at.
	 * 
	 * Canvases are sorted from low to high, meaning that higher values
	 * appear in front of lower values.
	 * @return The current layer value.
	 */
	int getLayer() { return m_layer; }

	/**
	 * @brief Sets the on which layer this Canvas will appear.
	 *
	 * Canvases are sorted from low to high, meaning that higher values
	 * appear in front of lower values.
	 * @param nl The new layer value.
	 */ 
	void setLayer(int nl) { m_layer = nl; }

	/**
	 * @brief Adds a widget to the Canvas.
	 *
	 * The widget will be added to the update and render queue of this Canvas
	 * so it will display and operate properly. The object is actually a handle
	 * that is used to retrieve the widget from a global list of widgets.
	 *
	 * @param w The widget handle from GuiSystem.
	 */
	void addWidget(int w);

	/**
	 * @brief Returns the clear color of the Canvas.
	 * @return The clear color as RGBA between 0.0f and 1.0f.
	 */
	Vector4 getClearColor() { return m_clearColor; }

	/**
	 * @brief Changes the clear color of the Canvas.
	 *
	 * @param vec The new clear color as RGBA between 0.0f and 1.0f
	 */
	void setClearColor(Vector4 vec) { m_clearColor = vec; }

	/**
	 * @brief Enables render to texture for the Canvas.
	 *
	 * When render to texture is enabled the canvas will not render directly
	 * to the screen anymore. It will draw everything to the specified texture
	 * in memory instead which can be used as a widget of some sort or some
	 * texture in the game.
	 *
	 * Attention: This will not overwrite the texture file on disk!
	 * It merely uses it to identify the surface to render to!
	 *
	 * @param tex The filename of the texture to render to.
	 */
	void enableRenderToTexture(const char* tex);

	/**
	 * @brief Enables the render to texture flag.
	 * 
	 * This is useful to temporarily suspend and resume rendering to a texture
	 * when Canvas::enableRenderToTexture(const char* text) was already called.
	 *
	 * @see Canvas::enableRenderToTexture
	 */
	void enableRenderToTexture()
	{
		if (m_texture != NULL)
			m_renderToTexture = true;
	}

	/**
	 * @brief Disables the render to texture flag.
	 * 
	 * This is useful to temporarily suspend and resume rendering to a texture
	 * when Canvas::enableRenderToTexture(const char* text) was already called.
	 *
	 * @see Canvas::enableRenderToTexture
	 */
	void disableRenderToTexture() { m_renderToTexture = false; }

	/**
	 * @brief Sets the camera offset value that emulates a basic 2D camera.
	 *
	 * @param value The new offset that should be applied.
	 */
	void setCameraOffset(Vector2 value) { m_cameraPosition = value; }

	/**
	 * @brief Returns the current camera offset.
	 * @return The current camera offset.
	 */
	Vector2 getCameraOffset() { return m_cameraPosition; }
};
}
}
#endif
