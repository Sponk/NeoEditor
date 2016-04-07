#ifndef __EVENT_H
#define __EVENT_H

#include <functional>
#include <Neo2D.h>

namespace Neo2D
{
namespace Gui
{
class Widget;
class NEO2D_EXPORT Event
{
	Widget& m_receiver;
	std::function<void(Widget&, const Event&, void*)> m_callback;
	void* m_data;

public:
	Event(Widget& w, std::function<void(Widget&, const Event&, void*)> cb, void* d) 
		: m_receiver(w),
		m_callback(cb),
		m_data(d) {}

	virtual ~Event() {}

	virtual void update(float dt) = 0;
	virtual void handle();
	virtual unsigned int getType() const { return 0; }

	Widget& getReceiver() const { return m_receiver; }
};

}
}

#endif //__EVENT_H
