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

#include <ThemedButton.h>
#include <Render.h>
#include <Neo2DEngine.h>
#include <Window/Mouse.h>

using namespace Neo2D;
using namespace Gui;

ThemedButton::ThemedButton(unsigned int x, unsigned int y, unsigned int width,
	unsigned int height, const char* label) :
	Button(x, y, width, height, label),
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

void ThemedButton::loadSprites(Sprite* sprites[], const char* vert,
	const char* hor, const char* body,
	const char* edge)
{
	sprites[TOP] = new Sprite(m_x, m_y, m_width, 0, hor, "");
	m_marginTop = sprites[TOP]->getSize().y;

	sprites[BOTTOM] = new Sprite(m_x, m_y + m_height + m_marginTop, m_width, 0, hor, "");

	sprites[LEFT] = new Sprite(m_x, m_y + m_marginTop, 0, m_height, vert, "");
	m_marginSide = sprites[LEFT]->getSize().x;

	sprites[RIGHT] = new Sprite(m_x + m_width + m_marginSide, m_y + m_marginTop, 0, m_height, vert, "");

	sprites[BODY] = new Sprite(m_x + m_marginSide, m_y + m_marginTop, m_width, m_height, body, "");

	sprites[TOP_LEFT] = new Sprite(m_x, m_y, 0, 0, edge, "");
	sprites[TOP_RIGHT] = new Sprite(m_x + m_width + m_marginTop, m_y, 0, 0, edge, "");

	sprites[BOTTOM_LEFT] = new Sprite(m_x, m_y + m_height + m_marginSide, 0, 0, edge, "");
	sprites[BOTTOM_RIGHT] = new Sprite(m_x + m_width + m_marginSide, m_y + m_height + m_marginTop, 0, 0, edge, "");

	sprites[TOP]->translate(Vector2(m_marginSide, 0));
	sprites[BOTTOM]->translate(Vector2(m_marginSide, 0));

	sprites[TOP]->setRotation(180.0);
	sprites[TOP_RIGHT]->setRotation(180);
	sprites[TOP_LEFT]->setRotation(90);

	sprites[RIGHT]->setRotation(-180);
	sprites[BOTTOM_RIGHT]->setRotation(-90);
}

void ThemedButton::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	Neo2DEngine* gui = Neo2DEngine::getInstance();

	// Calculate offset
	Widget::draw(offset);
	offset += getPosition();

	if (m_labelText == NULL)
	{
		m_labelText = render->createText(gui->getDefaultFont(),
			m_fontSize);
		m_labelText->setAlign(TEXT_ALIGN_CENTER);

		// Make variable!
		m_labelText->setColor(Vector3(0, 0, 0));
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
		color = Neo2DEngine::getInstance()->getNormalBackground();
		drawSprites(m_sprites);
		break;

	case BUTTON_HOVER_STATE:
		drawSprites(m_hoveredSprites);
		color = Neo2DEngine::getInstance()->getHoverBackground();
		break;

	case BUTTON_PRESSED_STATE:
		drawSprites(m_pressedSprites);
		color = Neo2DEngine::getInstance()->getHighlightBackground();
		break;
	}

	render->drawText(m_labelText, offset.x + 0.5 * (float)m_width,
		offset.y + 0.5 * m_labelText->getSize() +
		0.5 * static_cast<float>(m_height),
		m_rotation);
}

void ThemedButton::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	unsigned int x = 0;
	unsigned int y = 0;
	Vector2 res = system->getScreenSize();
	x = input->getAxis("MOUSE_X") * res.x;
	y = input->getAxis("MOUSE_Y") * res.y;

	unsigned int wx = m_offset.x + m_x;
	unsigned int wy = m_offset.y + m_y;

	if (m_state == BUTTON_PRESSED_STATE && input->onKeyUp("MOUSE_BUTTON_LEFT"))
	{
		doCallback();
		m_state = BUTTON_NORMAL_STATE;
	}

	if (m_state == BUTTON_PRESSED_STATE || (m_state == BUTTON_HOVER_STATE && input->onKeyDown("MOUSE_BUTTON_LEFT")))
	{
		m_state = BUTTON_PRESSED_STATE;
	   	return;
	}
	
	if (x >= wx && x <= wx + m_width + m_marginSide
		&& y >= wy && y <= wy + m_height + m_marginTop)
	{
		m_state = BUTTON_HOVER_STATE;
	}
	else
	{
		m_state = BUTTON_NORMAL_STATE;
	}
}

void ThemedButton::drawSprites(Sprite* sprites[])
{
	for (int i = 0; i < NUM_SPRITES; i++)
		sprites[i]->draw(m_offset);
}
