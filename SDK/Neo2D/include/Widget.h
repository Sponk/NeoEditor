#ifndef __WIDGET_H
#define __WIDGET_H

#include "Neo2D.h"
#include "Object2D.h"
#include "Event.h"
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT Widget : public Object2D
{
	//std::vector<shared_ptr<Event>> m_events;

	struct EventHandler
	{
		EventHandler() {}
		EventHandler(shared_ptr<Event> e) : event(e), data(nullptr), callback(nullptr) {}
		EventHandler(shared_ptr<Event> e, std::function<void(Widget&, const Event&, void*)> cb) : event(e), callback(cb), data(nullptr) {}
		EventHandler(shared_ptr<Event> e, std::function<void(Widget&, const Event&, void*)> cb, void* d) : event(e), callback(cb), data(d) {}

		shared_ptr<Event> event;
		std::function<void(Widget&, const Event&, void*)> callback;
		void* data;
	};

	std::map<unsigned int, EventHandler> m_events;

public:
	Widget(int x, int y, unsigned int w, unsigned int h, shared_ptr<Object2D> parent)
		: Object2D(x, y, w, h, parent)
	{}

	void draw(const Neo::Vector2&) {}
	void update(float dt)
	{
		if(!getParent())
			for(auto e : m_events)
				e.second.event->update(*this, dt);
	}

	virtual void handle(Event& e)
	{
		const auto iter = m_events.find(e.getType());
		if(iter == m_events.end())
			return;

		iter->second.callback(*this, e, iter->second.data);
	}

	void registerEvent(shared_ptr<Event> e)
	{
		registerEvent(e, nullptr, nullptr);
	}

	void registerEvent(shared_ptr<Event> e, std::function<void(Widget&, const Event&, void*)> cb, void* d)
	{
		m_events[e->getType()] = EventHandler(e, cb, d);

		if(getParent())
			dynamic_cast<Widget*>(getParent().get())->registerEvent(e, cb, d);
	}
};

}
}

#endif //__WIDGET_H
