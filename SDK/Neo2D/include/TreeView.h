#ifndef NEO_TREEVIEW_H
#define NEO_TREEVIEW_H

#include <Button.h>
#include <Widget.h>
#include <vector>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a tree which displays hierarchical structures.
 */
class NEO2D_EXPORT TreeView : public Widget, public enable_shared_from_this<TreeView>
{
public:

	/**
	 * @brief Implements a node in a tree.
	 */
	class TreeNode : public Widget, public std::enable_shared_from_this<TreeNode>
	{
		friend Neo2D::Gui::TreeView;
		std::vector<shared_ptr<TreeNode>> m_children;
		bool m_open;

		Button m_button;

	public:
		TreeNode(int x,
				 int y,
				 unsigned int w,
				 unsigned int h,
				 const char* label,
				 const shared_ptr<Object2D>& parent,
				 const shared_ptr<Theme>& theme);

		virtual bool handle(const Event& e) override;

		/**
		 * @brief Retrieves the list of children.
		 * @return The list of children.
		 */
		const std::vector<shared_ptr<TreeNode>>& getChildren() { return m_children; }

		/**
		 * @brief Adds a new child to the internal list of children.
		 * @param label The label to assign.
		 * @return The new TreeNode.
		 */
		shared_ptr<TreeNode> addChild(const char* label)
		{
			m_children.push_back(make_shared<TreeNode>(0, 0, 0, 0, label, shared_from_this(), getTheme()));
			m_children.back()->setCallback(getCallback(), nullptr);
			return m_children.back();
		}

		/**
		 * @brief Clears the sub-tree i.e. children.
		 */
		void clear() { m_children.clear(); }
		virtual void update(float dt) override
		{
			if(m_children.size())
			{
				m_button.setPosition(getPosition());
				m_button.update(dt);
			}

			Widget::update(dt);
			if(isOpen())
				for(auto c : m_children) c->update(dt);
		}

		virtual void draw(const Neo::Vector2& offset) override
		{
			Widget::draw(offset);

			if(m_children.size())
				m_button.draw(offset);
		}

		/**
		 * @brief Check if the node is considered open.
		 * @return A boolean value.
		 */
		bool isOpen() const { return m_open; }

		/**
		 * @brief Changes the open state.
		 * @param b A boolean value.
		 */
		void setOpen(bool b) { m_open = b; }

		shared_ptr<TreeNode> findNode(const char* name)
		{
			if(!strcmp(getLabel(), name))
				return shared_from_this();

			for(auto c : m_children)
				if(auto retval = c->findNode(name))
					return retval;

			return nullptr;
		}
	};

private:
	shared_ptr<TreeNode> m_rootNode;
	TreeNode* m_selected;
	shared_ptr<Theme> m_itemTheme;

	/**
	 * @brief Draws a tree starting from the given root.
	 *
	 * @param root The current node whos tree to draw.
	 * @param level The current "offset" in the tree.
	 * @param offset The offset to apply at the end to the position.
	 */
	void drawTree(TreeNode& root, Neo::Vector2& level, const Neo::Vector2& offset);

public:
	TreeView(int x,
			 int y,
			 unsigned int w,
			 unsigned int h,
			 const shared_ptr<Object2D>& parent,
			 const shared_ptr<Theme>& itemtheme = nullptr,
			 const shared_ptr<Theme>& background = nullptr);

	/**
	 * @brief Retrieves the root node.
	 * @return The root node.
	 */
	shared_ptr<TreeNode> getRoot()
	{
		if (m_rootNode == nullptr)
		{
			m_rootNode = make_shared<TreeNode>(0, 0, 0, 0, nullptr, shared_from_this(), m_itemTheme);
			m_rootNode->setCallback([this](Widget& w, void* d) {
				m_selected = static_cast<TreeNode*>(&w);
				doCallback();
			}, nullptr);
		}
		return m_rootNode;
	}

	const char* getSelected() const
	{
		if(m_selected)
			return m_selected->getLabel();

		return nullptr;
	}

	void setSelected(const char* label);
	shared_ptr<TreeNode> findNode(const char* name);

	virtual void update(float dt);
	virtual void draw(const Neo::Vector2& offset);
};

}
}

#endif //NEO_TREEVIEW_H
