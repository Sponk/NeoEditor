#ifndef __MOUSEEVENTS_H
#define __MOUSEEVENTS_H

#include "Neo2D.h"
#include "Event.h"
#include <NeoCore.h>
#include <NeoEngine.h>

namespace Neo2D
{
namespace Gui
{

enum MOUSE_EVENT_TYPES
{
	MOUSE_OVER = 1,
	MOUSE_LEAVE = 2,
	MOUSE_LEFT_CLICK = 3,
	MOUSE_MIDDLE_CLICK = 4,
	MOUSE_RIGHT_CLICK = 5,
	MOUSE_SCROLL = 6,
	MOUSE_LEFT_RELEASE = 7,
	MOUSE_MIDDLE_RELEASE = 8,
	MOUSE_RIGHT_RELEASE = 9
};

class NEO2D_EXPORT MouseOverEvent : public Event
{
	bool mouseOver = false;
public:
	MouseOverEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d) 
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::Mouse& mouse = Neo::NeoEngine::getInstance()->getInputContext()->getMouse();

		if(!mouseOver)
		{
			if((mouseOver = getReceiver().contains(mouse.getPosition())) == true)
				handle();
		}
		else
			mouseOver = getReceiver().contains(mouse.getPosition());
	}

	virtual unsigned int getType() const { return MOUSE_OVER; }
};

class NEO2D_EXPORT MouseLeaveEvent : public Event
{
	bool mouseOver = false;
public:
	MouseLeaveEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::Mouse& mouse = Neo::NeoEngine::getInstance()->getInputContext()->getMouse();
		if(mouseOver)
		{
			if((mouseOver = getReceiver().contains(mouse.getPosition())) == false)
				handle();
		}
		else
			mouseOver = getReceiver().contains(mouse.getPosition());
	}

	virtual unsigned int getType()  const { return MOUSE_LEAVE; }
};

class NEO2D_EXPORT MouseLeftClickEvent : public Event
{
public:
	MouseLeftClickEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d) 
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if(input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_LEFT) && getReceiver().contains(mouse.getPosition()))
			handle();
	}

	virtual unsigned int getType()  const { return MOUSE_LEFT_CLICK; }
};

class NEO2D_EXPORT MouseMiddleClickEvent : public Event
{
public:
	MouseMiddleClickEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if(input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_MIDDLE) && getReceiver().contains(mouse.getPosition()))
			handle();
	}

	virtual unsigned int getType()  const { return MOUSE_MIDDLE_CLICK; }
};

class NEO2D_EXPORT MouseRightClickEvent : public Event
{
public:
	MouseRightClickEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d) 
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if(input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_RIGHT) && getReceiver().contains(mouse.getPosition()))
			handle();
	}

	virtual unsigned int getType() const { return MOUSE_RIGHT_CLICK; }
};

class NEO2D_EXPORT MouseLeftReleaseEvent : public Event
{
public:
	MouseLeftReleaseEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if (input->getMouse().onKeyUp(Neo::MOUSE_BUTTON_LEFT) && getReceiver().contains(mouse.getPosition()))
			handle();
	}

	virtual unsigned int getType() const { return MOUSE_LEFT_RELEASE; }
};

class NEO2D_EXPORT MouseMiddleReleaseEvent : public Event
{
public:
	MouseMiddleReleaseEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if (input->getMouse().onKeyUp(Neo::MOUSE_BUTTON_MIDDLE) && getReceiver().contains(mouse.getPosition()))
			handle();
	}

	virtual unsigned int getType()  const { return MOUSE_MIDDLE_RELEASE; }
};

class NEO2D_EXPORT MouseRightReleaseEvent : public Event
{
public:
	MouseRightReleaseEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if (input->getMouse().onKeyUp(Neo::MOUSE_BUTTON_RIGHT) && getReceiver().contains(mouse.getPosition()))
			handle();
	}

	virtual unsigned int getType()  const { return MOUSE_RIGHT_RELEASE; }
};

class NEO2D_EXPORT MouseMoveEvent : public Event
{
	Neo::Vector2 delta;
	Neo::Vector2 lastPosition = Neo::Vector2(0,0);
public:
	MouseMoveEvent(Neo2D::Gui::Widget& p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();
		Neo::Vector2 mpos = mouse.getPosition();

		if(mpos != lastPosition)
		{
			delta = mpos - lastPosition;
			lastPosition = mpos;
			handle();
		}
	}

	const Neo::Vector2 &getDelta() const
	{
		return delta;
	}

	const Neo::Vector2 &getLastPosition() const
	{
		return lastPosition;
	}

	virtual unsigned int getType() { return MOUSE_MOVED; }
};

}
}

#endif