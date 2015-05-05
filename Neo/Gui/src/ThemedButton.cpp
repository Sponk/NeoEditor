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

#include <ThemedButton.h>
#include <Render.h>
#include <GuiSystem.h>
#include <Window/Mouse.h>

using namespace Neo::Gui;

ThemedButton::ThemedButton(unsigned int x, unsigned int y, unsigned int width,
						   unsigned int height, const char* label)
	: Button(x, y, width, height, label)
{
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		m_sprites[i] = NULL;
		m_hoveredSprites[i] = NULL;
		m_pressedSprites[i] = NULL;
	}
}

void ThemedButton::loadSprites(Sprite* sprites[], const char* vert,
							   const char* hor, const char* body,
							   const char* edge)
{
		sprites[TOP] = new Sprite(m_x, m_y, m_width, 0, hor, "");
		sprites[BOTTOM] = new Sprite(m_x, m_y+m_height, m_width, 0, hor, "");

		sprites[LEFT] = new Sprite(m_x, m_y, 0, m_height, vert, "");
		sprites[RIGHT] = new Sprite(m_x+m_width, m_y, 0, m_height, vert, "");

		
		sprites[BODY] = new Sprite(m_x, m_y, m_width, m_height, body, "");

		sprites[TOP_LEFT] = new Sprite(m_x, m_y, 0, 0, edge, "");
		sprites[TOP_RIGHT] = new Sprite(m_x+m_width, m_y, 0, 0, edge, "");

		sprites[BOTTOM_LEFT] = new Sprite(m_x, m_y+m_height, 0, 0, edge, "");
		sprites[BOTTOM_RIGHT] = new Sprite(m_x+m_width, m_y+m_height, 0, 0, edge, "");		
		
		sprites[RIGHT]->setFlip(Vector2(-1.0,-1.0));
		sprites[BOTTOM]->setFlip(Vector2(-1.0f, -1.0f));
		sprites[TOP_RIGHT]->setFlip(Vector2(-1,1));
		sprites[BOTTOM_LEFT]->setFlip(Vector2(1,-1));
		sprites[BOTTOM_RIGHT]->setFlip(Vector2(-1,-1));
}

void ThemedButton::draw()
{
	Render* render = Render::getInstance();
	GuiSystem* gui = GuiSystem::getInstance();

	if (m_labelText == NULL)
	{
		m_labelText = render->createText(gui->getDefaultFont(),
										 gui->getDefaultFontSize());
		m_labelText->setAlign(TEXT_ALIGN_CENTER);
	}

	if (m_sprites[0] == NULL)
	{

		loadSprites(m_sprites, gui->getThemeBorderV(), gui->getThemeBorderH(),
					gui->getThemeBody(), gui->getThemeEdge());

		loadSprites(m_hoveredSprites, gui->getThemeBorderVHover(),
					gui->getThemeBorderHHover(), gui->getThemeBodyHover(),
					gui->getThemeEdgeHover());

		loadSprites(m_pressedSprites, gui->getThemeBorderVPressed(),
					gui->getThemeBorderHPressed(), gui->getThemeBodyPressed(),
					gui->getThemeEdgePressed());

	}

	m_labelText->setText(m_label.c_str());

	Vector4 color;
	switch (m_state)
	{
		case BUTTON_NORMAL_STATE:
			color = GuiSystem::getInstance()->getNormalBackground();
			drawSprites(m_sprites);
			break;

		case BUTTON_HOVER_STATE:
			drawSprites(m_hoveredSprites);
			color = GuiSystem::getInstance()->getHoverBackground();
			break;

		case BUTTON_PRESSED_STATE:
			drawSprites(m_pressedSprites);
			color = GuiSystem::getInstance()->getHighlightBackground();
			break;
	}

	Box3d* box = m_labelText->getBoundingBox();
	//render->drawColoredQuad(m_x, m_y, m_width, m_height, color, m_rotation);
	render->drawText(m_labelText, m_x + 0.5 * (float) m_width,
					 m_y + (box->max.y - box->min.y) + 0.5 * (float) m_height,
					 m_rotation);
}


void ThemedButton::drawSprites(Sprite* sprites[])
{
	for(int i = 0; i < NUM_SPRITES; i++)
		sprites[i]->draw(Vector2(0,0));
}
