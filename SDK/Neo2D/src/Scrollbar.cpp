#include <Scrollbar.h>
#include <CommonEvents.h>
#include <Neo2DLevel.h>

using namespace Neo;

class KnobTheme : public Neo2D::Gui::Theme
{
public:

	KnobTheme()
	{}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();

		const Neo::Vector2 position = widget->getPosition() + offset;
		renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
		renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);
		//renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(1, 0, 0, 1), 0);
	}
};

class BackgroundTheme : public Neo2D::Gui::Theme
{
public:

	BackgroundTheme()
	{}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();

		const Neo::Vector2 position = widget->getPosition() + offset;
		renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
		renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.9, 0.9, 0.9, 1), 0);
	}
};

class GeneralMouseLeftReleaseEvent : public Neo2D::Gui::Event
{
public:
	GeneralMouseLeftReleaseEvent(Neo2D::Gui::Widget& w,
								 const function<void(Neo2D::Gui::Widget&, const Neo2D::Gui::Event&, void*)>& cb,
								 void* d)
		: Event(w, cb, d)
	{ }

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if (input->getMouse().onKeyUp(Neo::MOUSE_BUTTON_LEFT))
			handle();
		else
			reject();
	}

	virtual unsigned int getType() const { return Neo2D::Gui::MOUSE_LEFT_RELEASE; }
};

Neo2D::Gui::Scrollbar::Scrollbar(int x, int y, unsigned int w, unsigned int h,
				 const shared_ptr<Object2D>& parent,
				 const SCROLLBAR_DIRECTION direction,
				 const shared_ptr<Theme>& knobtheme,
				 const shared_ptr<Theme>& background)
	: Widget(x, y, w, h, nullptr, parent, (background == nullptr) ? make_shared<BackgroundTheme>() : background),
	  range(0, 100),
	  direction(direction),
	  value(0.0),
	  knob(x,y,w,h,*this, (knobtheme == nullptr) ? make_shared<KnobTheme>() : knobtheme)
{
	registerEvent(make_shared<MouseLeftClickEvent>(knob, nullptr, nullptr));
	registerEvent(make_shared<MouseLeaveEvent>(knob, nullptr, nullptr));
	registerEvent(make_shared<MouseMoveEvent>(knob, nullptr, nullptr));
	registerEvent(make_shared<GeneralMouseLeftReleaseEvent>(knob, nullptr, nullptr));
}

bool Neo2D::Gui::Scrollbar::handle(const Neo2D::Gui::Event & e)
{
	switch(e.getType())
	{
		case MOUSE_MOVED:
		{
			if (getState() != WIDGET_SELECTED)
				return false;

			auto delta = static_cast<const MouseMoveEvent&>(e).getDelta();
			float length = (direction == SCROLLBAR_HORIZONTAL) ? getSize().x : getSize().y;

			if(getDirection() == SCROLLBAR_HORIZONTAL)
			{
				value = (MAX(0, MIN(getRange().y - length, (getRange().y / length) * delta.x + getValue())));
				knob.setPosition(Neo::Vector2(getPosition().x + (getValue() / getRange().y) * getSize().x, knob.getPosition().y));
			}
			else
			{
				value = (MAX(0, MIN(getRange().y - length, (getRange().y / length) * delta.y + getValue())));
				knob.setPosition(Neo::Vector2(knob.getPosition().x, getPosition().y + (getValue() / getRange().y) * getSize().y));
			}
		}
		return true;

		case MOUSE_LEFT_CLICK:
			setState(WIDGET_SELECTED);
			return true;

		case MOUSE_LEFT_RELEASE:
			setState(WIDGET_NORMAL);
			return true;
	}

	return false;
}
