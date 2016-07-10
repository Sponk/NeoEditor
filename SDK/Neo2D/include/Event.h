#ifndef __EVENT_H
#define __EVENT_H

#include <functional>
#include <Neo2D.h>
#include <memory>

namespace Neo2D
{
namespace Gui
{
class Widget;

using std::weak_ptr;
using std::shared_ptr;

/**
 * @brief Implements a general event.
 *
 * An event will call the Widget::handle method of the widget it belongs to when the trigger criterium is met.
 * Additionally the callback will be called.
 *
 * When a widget gets deleted it will also remove all its registered events automatically.
 * @see Widget::~Widget
 */
class NEO2D_EXPORT Event
{
	weak_ptr<Widget>m_receiver;
	std::function<void(Widget&, const Event&, void*)> m_callback;
	void* m_data;
	bool m_handled;

public:

	/**
	 * @brief Constructs a new event.
	 * @param w The parent widget.
	 * @param cb The callback to call when triggered.
	 * @param d The data to pass to the callback when triggered.
	 */
	Event(weak_ptr<Widget> w, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: m_receiver(w),
		m_callback(cb),
		m_data(d),
		m_handled(false) {}

	virtual ~Event() {}

	/**
	 * @brief Updates the event.
	 *
	 * When the trigger criterium is met this method calls Event::handle, if not it calls
	 * Event::reject to indicate whether the event was handled or not.
	 *
	 * @param dt The frame delta time.
	 */
	virtual void update(float dt) { reject(); }

	/**
	 * @brief Notifies the parent widget about an event and calls the callback.
	 */
	virtual void handle();

	/**
	 * @brief Retrieves the event type.
	 * @return The type ID.
	 */
	virtual unsigned int getType() const { return 0; }

	/**
	 * @brief Indicates that this event wasn't handled after the last call to Event::update.
	 */
	void reject() { m_handled = false; }

	/**
	 * @brief Check if the event got handled after the last call to Event::update.
	 * @return A boolean value.
	 */
	bool handled() const { return m_handled; }

	/**
	 * @brief Retrieves the receiving widget.
	 * @return The widget reference.
	 */
	shared_ptr<Widget> getReceiver() const { return m_receiver.lock(); }
};

}
}

#endif //__EVENT_H
