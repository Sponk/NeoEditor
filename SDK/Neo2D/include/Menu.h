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
	MenuItem(const char* label, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr)
		: Button(0,0,0,0, label, parent, theme),
		  m_visible(false)
	{
		setActive(false);
	}

	virtual void handle(const Event& event) override;

	bool isVisible() const { return m_visible; }
	virtual void setVisible(bool b) { m_visible = b; }

	void hideHierarchy()
	{
		setVisible(false);
		setActive(false);

		if(!getParent().expired())
		{
			shared_ptr<Object2D> parent = getParent().lock();
			if(parent != nullptr)
			{
				auto menu = static_pointer_cast<MenuItem>(parent);
				menu->hideHierarchy();
			}
		}
	}
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
				m->setPosition(getPosition() + offset + Neo::Vector2(getParent().expired() ? 0 : getSize().x, line));
				m->setSize(getSize()); // FIXME: Don't hardcode this!
				line += getSize().y + 1;

				m->draw(Neo::Vector2());
			}
		}

		if(!getParent().expired())
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
			MenuItem::update(dt);
		}
	}

	virtual void setVisible(bool b) override
	{
		MenuItem::setVisible(b);
	}

	virtual void hideChildren()
	{
		for(auto m : m_children)
		{
			/*auto sm = dynamic_pointer_cast<Submenu>(m);
			if(sm)
				sm->hideChildren();*/

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

class NEO2D_EXPORT Menubar : public Widget, public enable_shared_from_this<Menubar>
{
	// FIXME: Don't hardcode!!
	const unsigned int BTN_WIDTH = 70;

	struct MenuButton
	{
		shared_ptr<Submenu> submenu;
		shared_ptr<Button> button;
	};

	std::vector<MenuButton> m_children;

public:
	Menubar(int x, int y, unsigned int w, unsigned int h,
			const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr);

	void addMenu(const shared_ptr<Submenu>& menu)
	{
		MenuButton menubutton;
		menubutton.submenu = menu;
		menubutton.button = make_shared<Button>(BTN_WIDTH * m_children.size() + getPosition().x, getPosition().y,
												BTN_WIDTH, getSize().y, menu->getLabel(), shared_from_this());

		menu->setPosition(menubutton.button->getPosition() + Neo::Vector2(0, getSize().y));

		menubutton.button->setCallback([] (Widget& w, void* data) {
			Submenu* menu = reinterpret_cast<Submenu*>(data);

			menu->setVisible(!menu->isVisible());
			menu->setActive(menu->isVisible());

			menu->deselectChildren();

		}, menu.get());

		menu->setVisible(false);
		m_children.push_back(menubutton);
	}

	virtual void update(float dt) override
	{
		Widget::update(dt);
		//for(auto menu : m_children)
		//{
			//menu.submenu->update(dt);
		//}
	}

	virtual void draw(const Neo::Vector2& offset) override
	{
		Widget::draw(offset);

		unsigned int x = getPosition().x + BTN_WIDTH * m_children.size();

		for(auto menu : m_children)
		{
			//menu.button->setPosition(Neo::Vector2(x, getPosition().y));
			//menu.button->setSize(Neo::Vector2(100, getSize().y));
			//menu.submenu->setPosition(Neo::Vector2(x, getPosition().y + getSize().y));

			menu.button->draw(offset);
			if(menu.submenu->isActive())
				menu.submenu->draw(offset);
		}
	}
};

}
}

#endif //NEO_MENU_H
