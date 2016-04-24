#ifndef NEO_MENU_H
#define NEO_MENU_H

#include <Neo2D.h>
#include <Button.h>
#include <Container.h>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT MenuItem : public Button
{
	bool m_visible;

public:
	/*MenuItem(const char* label, const shared_ptr<Theme>& theme = nullptr)
		: Button(0,0,0,0, label, nullptr, theme)
	{}*/
	MenuItem(const char* label, shared_ptr<Object2D> parent, shared_ptr<Theme> theme = nullptr)
		: Button(0,0,0,0, label, move(parent), move(theme)),
		  m_visible(false)
	{ }

	virtual void handle(const Event& event) override;

	bool isVisible() const { return m_visible; }
	virtual void setVisible(bool b) { m_visible = b; }
};

class NEO2D_EXPORT Submenu : public MenuItem, public std::enable_shared_from_this<Submenu>
{
	std::vector<std::shared_ptr<MenuItem>> m_children;

protected:
	void addItem(const std::shared_ptr<MenuItem>& i) { m_children.push_back(i); }

public:
	Submenu(const char* label, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr);

	std::shared_ptr<MenuItem> findChild(const std::string&) const;
	std::shared_ptr<Submenu> findSubmenu(const std::string& name) const
	{
		return dynamic_pointer_cast<Submenu>(findChild(name));
	}

	virtual void handle(const Event& e) override;
	virtual void draw(const Neo::Vector2& offset) override
	{
		if(isVisible())
		{
			size_t line = 0;
			for(auto m : m_children)
			{
				m->setPosition(getPosition() + offset + Neo::Vector2(getSize().x, line));
				m->setSize(getSize()); // FIXME: Don't hardcode this!
				line += getSize().y + 1;

				m->draw(Neo::Vector2());
			}
		}

		MenuItem::draw(offset);
	}

	virtual void update(float dt) override
	{
		if(isActive())
		{
      		for(auto m : m_children)
			{
				m->update(dt);
			}
		}

		MenuItem::update(dt);
	}

	virtual void setVisible(bool b) override
	{
		MenuItem::setVisible(b);
	}

	virtual void hideChildren()
	{
		for(auto m : m_children)
		{
			//auto sm = dynamic_pointer_cast<Submenu>(m);
			//if(sm)
			//	sm->hideChildren();

			m->setActive(false);
			m->setVisible(false);
		}
	}

	virtual void deselectChildren()
	{
		for(auto m : m_children)
		{
			auto sm = dynamic_pointer_cast<Submenu>(m);
			if(sm)
			{
				sm->hideChildren();
				sm->setVisible(false);
			}
		}
	}

	shared_ptr<MenuItem> addItem(const std::string& name, std::function<void(Widget&, void*)> cb);
};

class NEO2D_EXPORT Menubar : public Container
{
public:
	Menubar(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent)
		: Container(x, y, w, h, parent)
	{}
};

}
}

#endif //NEO_MENU_H
