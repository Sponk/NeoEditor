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

#include <ThemedInputField.h>
#include <Render.h>
#include <Neo2DEngine.h>
#include <Window/Mouse.h>

using namespace Neo2D;
using namespace Gui;

ThemedInputField::ThemedInputField(unsigned int x, unsigned int y, unsigned int width,
								   unsigned int height, const char* label) :
	InputField(x, y, width, height, label),
	m_marginTop(0),
	m_marginSide(0)

{
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		m_sprites[i] = NULL;
		m_hoveredSprites[i] = NULL;
		m_pressedSprites[i] = NULL;
	}

	m_fontSize = Neo2DEngine::getInstance()->getDefaultFontSize();
}

void ThemedInputField::loadSprites(Sprite* sprites[], const char* vert,
							   const char* hor, const char* body,
							   const char* edge)
{
		sprites[TOP] = new Sprite(m_x, m_y, m_width, 0, hor, "");
		m_marginTop = sprites[TOP]->getSize().y;

		sprites[BOTTOM] = new Sprite(m_x, m_y+m_height+m_marginTop, m_width, 0, hor, "");

		sprites[LEFT] = new Sprite(m_x, m_y+m_marginTop, 0, m_height, vert, "");
		m_marginSide = sprites[LEFT]->getSize().x;

		sprites[RIGHT] = new Sprite(m_x+m_width+m_marginSide, m_y+m_marginTop, 0, m_height, vert, "");
		
		sprites[BODY] = new Sprite(m_x + m_marginSide, m_y + m_marginTop, m_width, m_height, body, "");

		sprites[TOP_LEFT] = new Sprite(m_x, m_y, 0, 0, edge, "");
		sprites[TOP_RIGHT] = new Sprite(m_x+m_width+m_marginTop, m_y, 0, 0, edge, "");

		sprites[BOTTOM_LEFT] = new Sprite(m_x, m_y+m_height+m_marginSide, 0, 0, edge, "");
		sprites[BOTTOM_RIGHT] = new Sprite(m_x+m_width+m_marginSide, m_y+m_height+m_marginTop, 0, 0, edge, "");		
	
		sprites[TOP]->translate(Vector2(m_marginSide, 0));
		sprites[BOTTOM]->translate(Vector2(m_marginSide, 0));

		sprites[TOP]->setRotation(180.0);
		sprites[TOP_RIGHT]->setRotation(180);
		sprites[TOP_LEFT]->setRotation(90);

		sprites[RIGHT]->setRotation(-180);
		sprites[BOTTOM_RIGHT]->setRotation(-90);
}

void ThemedInputField::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	Neo2DEngine* gui = Neo2DEngine::getInstance();
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();

	offset += getPosition() + m_offset;

	Vector2 res = system->getScreenSize();

	if (m_labelText == NULL)
	{
		m_labelText = render->createText(gui->getDefaultFont(),
										 m_fontSize);
		m_labelText->setAlign(TEXT_ALIGN_LEFT);
		m_labelText->setColor(Vector3(0,0,0));
	}

	if (m_sprites[0] == NULL)
	{
		string themeDir = gui->getThemeDirectory();
		loadSprites(m_sprites, (themeDir + "/input-field-borderv.png").c_str(),
					(themeDir + "/input-field-borderh.png").c_str(),
					(themeDir + "/input-field-body.png").c_str(),
					(themeDir + "/input-field-edge.png").c_str());

		loadSprites(m_hoveredSprites, (themeDir + "/input-field-borderv-hovered.png").c_str(),
					(themeDir + "/input-field-borderh-hovered.png").c_str(),
					(themeDir + "/input-field-body-hovered.png").c_str(),
					(themeDir + "/input-field-edge-hovered.png").c_str());

		loadSprites(m_pressedSprites, (themeDir + "/input-field-borderv-pressed.png").c_str(),
					(themeDir + "/input-field-borderh-pressed.png").c_str(),
					(themeDir + "/input-field-body-pressed.png").c_str(),
					(themeDir + "/input-field-edge-pressed.png").c_str());

	}

	Vector4 color;
	switch (m_state)
	{
		case INPUT_NORMAL_STATE:
			color = 1.5 * Neo2DEngine::getInstance()->getNormalBackground();
			drawSprites(m_sprites);
			break;

		case INPUT_HOVER_STATE:
			color = 1.2 * Neo2DEngine::getInstance()->getHoverBackground();
			drawSprites(m_hoveredSprites);
			break;

		case INPUT_SELECTED_STATE:
			color = Neo2DEngine::getInstance()->getHighlightBackground();
			drawSprites(m_pressedSprites);
			break;
	}

	// Update text
	updateLabel(m_label);
	float textOffset = calculateWidth(m_labelText);
	Box3d* box = m_labelText->getBoundingBox();

	// Calculate overflow offset
	if (textOffset <= m_width) textOffset = 0.0f; // If we do not overflow, simply ignore
	else textOffset -= m_width; // Calculate the new position relative to the right edge

	RenderingContext* renderContext =
		NeoEngine::getInstance()->getRenderingContext();

	renderContext->enableScissorTest();
	renderContext->setScissor(offset.x, res.y - (offset.y + m_height), m_width, m_height);

	render->drawText(m_labelText, m_x - textOffset,
					 m_y + m_fontSize + m_marginTop,
					 m_rotation);

	renderContext->disableScissorTest();
}

void ThemedInputField::drawSprites(Sprite* sprites[])
{
	for(int i = 0; i < NUM_SPRITES; i++)
		sprites[i]->draw(Vector2(0,0));
}
