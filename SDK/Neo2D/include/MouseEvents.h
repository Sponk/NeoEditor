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
	MOUSE_SCROLL = 6
};

class NEO2D_EXPORT MouseOverEvent : public Event
{
	bool mouseOver = false;
public:
	virtual void update(Widget &w, float dt)
	{
		if(!mouseOver)
		{
			Neo::Mouse& mouse = NeoEngine::getInstance()->getInputContext()->getMouse();
			if((mouseOver = w.contains(mouse.getPosition())) == true)
				handle(w);
		}
		else
			mouseOver = w.contains(getMousePosition());
	}

	virtual unsigned int getType() { return 1; }
};

class NEO2D_EXPORT MouseLeaveEvent : public Event
{
	bool mouseOver = false;
public:
	virtual void update(Widget &w, float dt)
	{
		Neo::Mouse& mouse = NeoEngine::getInstance()->getInputContext()->getMouse();
		if(mouseOver)
		{
			if((mouseOver = w.contains(mouse.getPosition())) == false)
				handle(w);
		}
		else
			mouseOver = w.contains(mouse.getPosition());
	}
};

class NEO2D_EXPORT MouseLeftClickEvent : public Event
{
public:
	virtual void update(Widget &w, float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = NeoEngine::getInstance()->getInputContext()->getMouse();

		if(input->getMouse().onKeyDown(MOUSE_BUTTON_LEFT) && w.contains(mouse.getPosition()))
			handle(w);
	}
};

class NEO2D_EXPORT MouseMiddleClickEvent : public Event
{
public:
	virtual void update(Widget &w, float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = NeoEngine::getInstance()->getInputContext()->getMouse();

		if(input->getMouse().onKeyDown(MOUSE_BUTTON_MIDDLE) && w.contains(mouse.getPosition()))
			handle(w);
	}
};

class NEO2D_EXPORT MouseRightClickEvent : public Event
{
public:
	virtual void update(Widget &w, float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = NeoEngine::getInstance()->getInputContext()->getMouse();

		if(input->getMouse().onKeyDown(MOUSE_BUTTON_RIGHT) && w.contains(mouse.getPosition()))
			handle(w);
	}
};

class NEO2D_EXPORT MouseMoveEvent : public Event
{
	Neo::Vector2 delta;
	Neo::Vector2 lastPosition = Neo::Vector2(0,0);
public:
	virtual void update(Widget &w, float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = NeoEngine::getInstance()->getInputContext()->getMouse();
		Neo::Vector2 mpos = mouse.getPosition();

		if(mpos != lastPosition)
		{
			delta = mpos - lastPosition;
			lastPosition = mpos;
			handle(w);
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
};

}
}

#endif