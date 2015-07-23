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

#ifndef __TREE_H__
#define __TREE_H__

#include <string>
#include <NeoEngine.h>
#include <Widget.h>
#include <Button.h>
#include <Label.h>
#include <vector>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Represents a single tree node.
 * @author Yannick Pflanzer
 */
template<class T> class NEO2D_EXPORT TreeNode
{
protected:
	T m_data;
	bool m_opened;

	std::vector<TreeNode<T>*> m_children;

public:
	TreeNode* getChild(int idx) { return m_children[idx]; }
	TreeNode* addChild(T data)
	{
		TreeNode<T>* t = new TreeNode<T>(data);
		m_children.push_back(t);
		return t;
	}

	size_t getNumChildren(){ return m_children.size(); }
	TreeNode* getChild(size_t idx) { return m_children[idx]; }
	void clearChildren() { m_children.clear(); }

	T getData() { return m_data; }

	void setOpen(bool v) { m_opened = v; }
	bool isOpen() { return m_opened; }

	TreeNode(T d) : m_data(d), m_opened(false){}
	TreeNode() {}
	~TreeNode()	{}
};

#ifdef SWIG
// Some templates
%template(TreeModel) TreeNode<std::string>;
#endif

using namespace Neo;
	
/**
 * @brief
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT TreeView : public Widget
{
	friend class TreeNode<std::string>;
protected:

	TreeNode<std::string> m_root;
	TreeNode<std::string>* m_selected;

	Label* m_label;

	static void treeCallback(Widget* w, long int data);

	struct CallbackData
	{
		int idx;
		TreeView* list;
	};

	void updateTree(TreeNode<std::string>* root, Vector2* level);
	void drawTree(TreeNode<std::string>* root, Vector2* level);

public:
	TreeView(unsigned int x, unsigned int y, unsigned int width,
		  unsigned int height, const char* label)
		: Widget(x, y, width, height, label),
		  m_label(nullptr),
		  m_selected(nullptr)
	{}

	~TreeView() { clear(); }

	void show() { m_visible = true; }
	void clear(TreeNode<std::string>* root);
	void clear() { clear(&m_root); }

	//int getSelectedEntry() { return m_selectedEntry; }
	TreeNode<std::string>* getTreeModel() { return &m_root; }
	const char* getSelected() { if(m_selected) return m_selected->getData().c_str(); return nullptr; }

	void draw(Vector2 offset);
	void update();
};
}
}
#endif
