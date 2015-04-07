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

#ifndef __THEMED_BUTTON_H__
#define __THEMED_BUTTON_H__

#include <Button.h>
#include <Sprite.h>
#include <NeoEngine.h>

namespace Neo
{
namespace Gui
{

#define NUM_SPRITES 9
	
/**
 * @brief Implements a simple pushable button that calls the specified callback. This
 * implementation uses Sprites to become themable.
 *
 * @author Yannick Pflanzer
 */
class ThemedButton : public Button
{
	enum SPRITE_POS
	{
		BODY = 0,
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT
	};
	
	Sprite* m_sprites[NUM_SPRITES];
	Sprite* m_hoveredSprites[NUM_SPRITES];
	Sprite* m_pressedSprites[NUM_SPRITES];

    
	void loadSprites(Sprite* sprites[], const char* vert,
					 const char* hor, const char* body,
					 const char* edge);
	
	void drawSprites(Sprite* sprites[]);
public:
	ThemedButton(unsigned int x, unsigned int y, unsigned int width,
		   unsigned int height, const char* label);

	void draw();
};
}
}
#endif
