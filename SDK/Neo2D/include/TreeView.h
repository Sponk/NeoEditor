#ifndef NEO_TREEVIEW_H
#define NEO_TREEVIEW_H

#include <Button.h>
#include <Widget.h>
#include <vector>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT TreeView : public Widget, public enable_shared_from_this<TreeView>
{
public:

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

		virtual void handle(const Event& e);

		const std::vector<shared_ptr<TreeNode>>& getChildren() { return m_children; }

		shared_ptr<TreeNode> addChild(const char* label)
		{
			m_children.push_back(make_shared<TreeNode>(0, 0, 0, 0, label, shared_from_this(), getTheme()));
			m_children.back()->setCallback(getCallback(), nullptr);
			return m_children.back();
		}

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

		bool isOpen() const { return m_open; }
		void setOpen(bool b) { m_open = b; }
	};

private:
	shared_ptr<TreeNode> m_rootNode;
	TreeNode* m_selected;
	shared_ptr<Theme> m_itemTheme;

	void drawTree(TreeNode& root, Neo::Vector2& level, const Neo::Vector2& offset);

public:
	TreeView(int x,
			 int y,
			 unsigned int w,
			 unsigned int h,
			 const shared_ptr<Object2D>& parent,
			 const shared_ptr<Theme>& itemtheme = nullptr,
			 const shared_ptr<Theme>& background = nullptr);

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

	virtual void update(float dt);
	virtual void draw(const Neo::Vector2& offset);
};

}
}

#endif //NEO_TREEVIEW_H
