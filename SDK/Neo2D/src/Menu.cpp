#include <Menu.h>
#include <CommonEvents.h>
#include <string>
#include <sstream>
#include <vector>
#include <Neo2DLevel.h>
#include <NeoEngine.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

class SubmenuTheme : public Neo2D::Gui::Theme
{
	OText* text;
public:

	SubmenuTheme()
	{
		text = Neo2D::Neo2DLevel::getInstance()->createText("assets/default.ttf", 12);
		text->setAlign(TEXT_ALIGN_CENTER);
	}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();

		text->setText(widget->getLabel());
		const Neo::Vector2 position = widget->getPosition() + offset;

		switch(widget->getState())
		{
			case Neo2D::Gui::WIDGET_HOVER:
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.0, 0.0, 0.8, 1), 0);

				text->setColor(Vector4(1.0,1.0,1.0,1.0));
				draw2DText(text, position.x  + 0.5f * widget->getSize().x,
						   position.y + text->getSize() * 1.25, 0.0f);

				break;

			case Neo2D::Gui::WIDGET_NORMAL:
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);

				text->setColor(Vector4(0.0,0.0,0.0,1.0));
				draw2DText(text, position.x  + 0.5f * widget->getSize().x,
						   position.y + text->getSize() * 1.25, 0.0f);

				break;

			case Neo2D::Gui::WIDGET_SELECTED:
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.0, 0.0, 0.0, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,1), Vector4(0.4, 0.4, 0.4, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.0, 0.0, 0.8, 1), 0);

				text->setColor(Vector4(1.0,1.0,1.0,1.0));
				draw2DText(text, position.x  + 0.51f * widget->getSize().x,
						   position.y + text->getSize() * 1.25 + 0.1f * widget->getSize().y, 0.0f);

				break;

			default: break;
		}
	}

	void draw2DText(OText* text, float x, float y, float rotation)
	{
		RenderingContext* renderContext =
			NeoEngine::getInstance()->getRenderingContext();

		text->setPosition(Vector3(floor(x), floor(y), 0));
		text->setRotation(Quaternion(0, 0, rotation));
		text->updateMatrix();

		renderContext->pushMatrix();
		renderContext->multMatrix(text->getMatrix());

		NeoEngine::getInstance()->getRenderer()->drawText(text);
		renderContext->popMatrix();
	}
};

Submenu::Submenu(const char* label, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme)
	: MenuItem(label, parent, (theme == nullptr) ? make_shared<SubmenuTheme>() : theme)
{
	//registerEvent(std::make_shared<MouseDeselectEvent>(*this, nullptr, this));
	setSize(Vector2(200,20));
}

bool Submenu::handle(const Event& e)
{
	bool handled = Button::handle(e);
	switch(e.getType())
	{
		case MOUSE_OVER:
		{
			shared_ptr<Object2D> parent = getParent().lock();
			if(parent != nullptr)
			{
				static_pointer_cast<Submenu>(parent)->deselectChildren();
			}

			setVisible(true);
			for (auto m : m_children)
			{
				m->setActive(true);
				m->setVisible(true);

				shared_ptr<Submenu> sm;
				if ((sm = dynamic_pointer_cast<Submenu>(m)) != nullptr)
					sm->hideChildren();
			}
		}
		return true;

		case MOUSE_DESELECT:

			//setVisible(false);
			//setActive(false);

			/*for(auto m : m_children)
			{
				m->setActive(false);
				m->setVisible(false);

				shared_ptr<Submenu> sm;
				if((sm = dynamic_pointer_cast<Submenu>(m)) != nullptr)
					sm->hideChildren();
			}*/
			break;
	}

	return handled;
}

std::shared_ptr<MenuItem> Submenu::findChild(const std::string& name) const
{
	for(auto m : m_children)
		if(name == m->getLabel())
			return m;

	return nullptr;
}

shared_ptr<MenuItem> Submenu::addItem(const std::string& name, std::function<void(Widget&, void*)> cb)
{
	std::vector<std::string> path;
	std::stringstream ss(name);
	std::string line;

	path.reserve(32);
	while(std::getline(ss, line, '/'))
		if(!line.empty()) path.push_back(line);

	// Current always points to the parent
	shared_ptr<Submenu> currentShared = shared_from_this();

	for(int i = 0; i < path.size() - 1; i++)
	{
		const std::string& name = path[i];
		auto foundShared = currentShared->findSubmenu(name);

		if(foundShared != nullptr)
		{
			currentShared = foundShared;
		}
		else
		{
			auto newMenu = make_shared<Submenu>(name.c_str(), currentShared, getTheme());
			currentShared->addItem(newMenu);
			currentShared = newMenu;
		}
	}

	auto item = make_shared<MenuItem>(path.back().c_str(), currentShared, getTheme());
	item->setCallback(cb, nullptr);

	currentShared->addItem(item);
	return item;
}

bool MenuItem::handle(const Event& e)
{
	bool handled = Button::handle(e);
	if(e.getType() == MOUSE_LEFT_RELEASE)
	{
		hideHierarchy();
		return true;
	}
	return handled;
}

class MenubarTheme : public Theme
{
public:
	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset) override
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();
		const Neo::Vector2 position = widget->getPosition() + offset;

		renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
		renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);
	}
};

Menubar::Menubar(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme)
	: Widget(x, y, w, h, nullptr, parent, (theme == nullptr) ? make_shared<MenubarTheme>() : theme)
{}

void MenuItem::hideHierarchy()
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

			if(auto sm = dynamic_pointer_cast<Submenu>(menu))
				sm->hideChildren();
		}
	}
}
