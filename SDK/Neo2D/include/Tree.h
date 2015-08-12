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

	/**
	 * @brief Returns the child that has the given index.
	 * @param idx The index.
	 * @return The child.
	 */
	TreeNode<T>* getChild(int idx) { return m_children[idx]; }

	/**
	 * @brief Adds a child to the node.
	 * @param data The data of the new child node.
	 * @return A pointer to the new node.
	 */
	TreeNode<T>* addChild(T data)
	{
		TreeNode<T>* t = new TreeNode<T>(data);
		m_children.push_back(t);
		return t;
	}

	/**
	 * @brief Returns the number of children this node has.
	 * @return The number of children.
	 */
	size_t getNumChildren(){ return m_children.size(); }

	/**
	 * @brief Clears the list of children.
	 * Attention: Does not delete any child data!
	 */
	void clearChildren() { m_children.clear(); }

	/**
	 * @brief Returns the data of this node.
	 * @return The data.
	 */
	T getData() { return m_data; }

	/**
	 * @brief Configures if this node is considered to be opened up.
	 * Important for the tree view to decide whether or not tho show the children.
	 *
	 * @param v The boolean value.
	 */
	void setOpen(bool v) { m_opened = v; }

	/**
	 * @brief Checks if this node is open.
	 * @return A boolean value.
	 */
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
 * @brief Implements a tree view which displays a multi-way tree of strings
 * while providing callbacks for the selection of an entry.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT TreeView : public Widget
{
	friend class TreeNode<std::string>;
protected:

	bool m_autoSize;
	TreeNode<std::string> m_root;
	TreeNode<std::string>* m_selected;

	Label* m_label;

	/**
	 * @brief Walks down the tree and updates every node of it for detecting
	 * clicks on the small "+" signs and the entry itself.
	 *
	 * Invariant: The lower part of the tree is updated after one call to the method.
	 * Variant: The root node progresses further down the tree by one level and thus leads the
	 * algorithm to eventually terminate.
	 *
	 * @param root The node to start at.
	 * @param level The pointer to a Vector2 instance for calculating
	 * the (x|y) level of the currently handled node.
	 */
	void updateTree(TreeNode<std::string>* root, Vector2* level);

	/**
	 * @brief Walks down the tree and draws every node of it.
	 *
	 * Invariant: The lower part of the tree is drawn after one call to the method.
	 * Variant: The root node progresses further down the tree by one level and thus leads the
	 * algorithm to eventually terminate.
	 *
	 * @param root The node to start at.
	 * @param level The pointer to a Vector2 instance for calculating
	 * the (x|y) level of the currently handled node.
	 */
	void drawTree(TreeNode<std::string>* root, Vector2* level);

	TreeNode<std::string>* find(const char* name, TreeNode<std::string>* root);

public:
	TreeView(unsigned int x, unsigned int y, unsigned int width,
		  unsigned int height, const char* label)
		: Widget(x, y, width, height, label),
		m_label(nullptr),
		m_selected(nullptr),
		m_autoSize(true)
	{
			m_root.setOpen(true);
	}

	~TreeView() { clear(); }

	/**
	 * @brief Checks if the tree widget is configured to resize
	 * itself according to its content.
	 *
	 * @return A boolean value.
	 */
	bool getAutoSize() { return m_autoSize; }

	/**
	 * @brief Configures the tree widget to resize itself according to its content.
	 * @param b The boolean value.
	 */
	void setAutoSize(bool b) { m_autoSize = b; }
	
	/**
	 * @brief Shows the tree.
	 */
	void show() { m_visible = true; }

	/**
	 * @brief Clears the tree from the given node downwards.
	 *
	 * Attention: Deletes all nodes so they can't be used anymore!
	 *
	 * Invariant: The lower part of the tree is cleared after one call.
	 * Variant: The given root node walks down one level for every recursion
	 * therefore the algorithm terminates.
	 *
	 * @param The root node to start at.
	 */
	void clear(TreeNode<std::string>* root);

	/**
	 * @brief Clears everything starting at the root node.
	 * @see clear(TreeNode<std::string>* root)
	 */
	void clear() { clear(&m_root); }

	/**
	 * @brief Returns the root node of this tree.
	 * @return The root node.
	 */
	TreeNode<std::string>* getTreeModel() { return &m_root; }

	/**
	 * @brief Returns the currently selected node or nullptr if none is selected.
	 * @return The name of the selected node.
	 */
	const char* getSelected() { if(m_selected) return m_selected->getData().c_str(); return nullptr; }

	void selectEntry(const char* name);
	void selectEntry(TreeNode<std::string>* node) { m_selected = node; }

	TreeNode<std::string>* find(const char* name);

	void draw(Vector2 offset);
	void update();

	void setSize(Vector2 sz) { m_width = sz.x; if(!m_autoSize) m_height = sz.y; }
};
}
}
#endif
