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

#include <Tree.h>
#include <Render.h>
#include <Neo2DEngine.h>
#include <ThemedButton.h>

#define LINE_HEIGHT 15
#define BUTTON_SIZE 10

using namespace Neo2D;
using namespace Gui;

void TreeView::update()
{
	Vector2 level(0,-1);
	updateTree(&m_root, &level);

	if(m_autoSize)
	{
		m_width = getParent()->getSize().x;
		m_height = level.y * LINE_HEIGHT;
	}
}

void TreeView::draw(Vector2 offset)
{
	Render* render = Render::getInstance();
	Neo2DEngine* gui = Neo2DEngine::getInstance();

	if(m_label == nullptr)
	{
		m_label = new Label(0,0,BUTTON_SIZE,BUTTON_SIZE,"");
		m_label->setColor(Vector4(0,0,0,1));
	}

	m_offset = offset + getPosition();
	render->drawColoredQuad(m_offset.x, m_offset.y, m_width, m_height, Vector4(1,1,1,1));

	Vector2 level(0,-1);
	drawTree(&m_root, &level);
}

void TreeView::updateTree(TreeNode<std::string>* root, Vector2* level)
{
	if(root == nullptr)
		return;

	Vector2 pos = Vector2(m_x + m_offset.x, m_y + m_offset.y) + *level * LINE_HEIGHT;
	//m_label->setPosition(pos + Vector2(LINE_HEIGHT, 0));

	if(level->x > 0)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		SystemContext* system = engine->getSystemContext();
		InputContext* input = engine->getInputContext();

		unsigned int x = 0;
		unsigned int y = 0;
		Vector2 res = system->getScreenSize();
		x = input->getAxis("MOUSE_X") * res.x;
		y = input->getAxis("MOUSE_Y") * res.y;

		if(x <= pos.x + BUTTON_SIZE && x >= pos.x
			&& y <= pos.y + BUTTON_SIZE && y >= pos.y &&
			input->onKeyDown("MOUSE_BUTTON_LEFT"))
		{
			root->setOpen(!root->isOpen());
		}
		else if(x <= pos.x + m_width && x >= pos.x
				&& y <= pos.y + LINE_HEIGHT && y >= pos.y
				&& input->onKeyDown("MOUSE_BUTTON_LEFT"))
		{
			m_selected = root;
			doCallback();
		}

		// MLOG_INFO("x: " << pos.x << " y: " << pos.y << " mx: " << x << " my: " << y << " width: " << m_width);

		if(m_label)
			m_label->update();
	}

	level->y++;

	if(!root->isOpen())
		return;

	level->x++;
	for(int i = 0; i < root->getNumChildren(); i++)
	{
		updateTree(root->getChild(i), level);
	}
	level->x--;
}

void TreeView::drawTree(TreeNode<std::string>* root, Vector2* level)
{
	if(root == nullptr)
		return;

	Render* render = Render::getInstance();
	Vector2 pos = Vector2(m_offset.x, m_offset.y) + *level * LINE_HEIGHT;

	if(level->x > 0)
	{
		if(root == m_selected)
			render->drawColoredQuad(m_offset.x, pos.y, m_width, LINE_HEIGHT, Vector4(0.7,0.7,0.7,1.0));

		if(root->getNumChildren() > 0)
		{
			render->drawColoredQuad(pos.x + (LINE_HEIGHT - BUTTON_SIZE)/2.0, pos.y + (LINE_HEIGHT - BUTTON_SIZE)/2.0,
									BUTTON_SIZE, BUTTON_SIZE,  Vector4(0.8,0.8,0.8,1.0));

			m_label->setPosition(pos);
			m_label->setLabel((root->isOpen()) ? "-" : "+");
			m_label->draw(Vector2(0,0));
			//m_button->update();
			//m_button->draw(Vector2(0,0));
		}

		m_label->setPosition(pos + Vector2(LINE_HEIGHT, 0));
		m_label->setLabel(root->getData().c_str());
		m_label->draw(Vector2(0,0));
	}

	level->y++;

	if(!root->isOpen())
		return;

	level->x++;
	for(int i = 0; i < root->getNumChildren(); i++)
	{
		drawTree(root->getChild(i), level);
	}

	level->x--;
}

void TreeView::clear(TreeNode<std::string>* root)
{
	for(int i = 0; i < root->getNumChildren(); i++)
	{
		clear(root->getChild(i));
	}

	root->clearChildren();
	if(root != &m_root)
		SAFE_DELETE(root);

	m_selected = nullptr;
}

TreeNode<std::string>* TreeView::find(const char* name, TreeNode<std::string>* root)
{
	if(root == nullptr) return nullptr;

	if(root->getData() == name)
		return root;

	TreeNode<std::string>* entry;
	for(int i = 0; i < root->getNumChildren(); i++)
	{
		if((entry = find(name, root->getChild(i))) != nullptr)
			return entry;
	}

	return nullptr;
}

TreeNode<std::string>* TreeView::find(const char* name)
{
	return find(name, &m_root);
}

void TreeView::selectEntry(const char* name)
{
	m_selected = find(name);
}
