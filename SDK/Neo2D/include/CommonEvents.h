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

/**
 * @brief The most common types for GUI events.
 */
enum COMMON_EVENT_TYPES
{
	/// The mouse entered the widget.
	MOUSE_OVER = 1,
	/// The mouse left the widget.
	MOUSE_LEAVE = 2,
	/// The left mouse button was pressed.
	MOUSE_LEFT_CLICK = 3,
	/// The middle mouse button was pressed.
	MOUSE_MIDDLE_CLICK = 4,
	/// The right mouse button was pressed.
	MOUSE_RIGHT_CLICK = 5,
	/// The mouse scroll wheel was moved.
	MOUSE_SCROLL = 6,
	/// The left mouse button was released.
	MOUSE_LEFT_RELEASE = 7,
	/// The middle mouse button was released.
	MOUSE_MIDDLE_RELEASE = 8,
	/// The right mouse button was released.
	MOUSE_RIGHT_RELEASE = 9,
	/// The mouse moved.
	MOUSE_MOVE = 10,
	/// A key was pressed.
	KEY_PRESSED = 11,
	/// A key was released.
	KEY_RELEASED = 12,
	/// A UTF8 character was entered.
	CHARACTER_INPUT = 13,
	/// The mouse was used to deselect the widget.
	MOUSE_DESELECT = 14
};

/**
 * @brief Gets triggered each time the mouse cursor enters the parent widget area.
 */
class NEO2D_EXPORT MouseOverEvent : public Event
{
	bool mouseOver = false;
public:
	MouseOverEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::Mouse& mouse = Neo::NeoEngine::getInstance()->getInputContext()->getMouse();
		reject();

		if(!mouseOver)
		{
			if((mouseOver = getReceiver()->contains(mouse.getPosition())) == true)
				handle();
		}
		else
			mouseOver = getReceiver()->contains(mouse.getPosition());
	}

	virtual unsigned int getType() const { return MOUSE_OVER; }
};

/**
 * @brief Gets triggered each time the mouse cursor leaves the parent widget area.
 */
class NEO2D_EXPORT MouseLeaveEvent : public Event
{
	bool mouseOver = false;
public:
	MouseLeaveEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		reject();
		Neo::Mouse& mouse = Neo::NeoEngine::getInstance()->getInputContext()->getMouse();
		if(mouseOver)
		{
			if((mouseOver = getReceiver()->contains(mouse.getPosition())) == false)
				handle();
		}
		else
			mouseOver = getReceiver()->contains(mouse.getPosition());
	}

	virtual unsigned int getType()  const { return MOUSE_LEAVE; }
};

/**
 * @brief Gets triggered each time the mouse left clicks on the parent widget area.
 */
class NEO2D_EXPORT MouseLeftClickEvent : public Event
{
public:
	MouseLeftClickEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if(input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_LEFT) && getReceiver()->contains(mouse.getPosition()))
			handle();
		else
			reject();
	}

	virtual unsigned int getType()  const { return MOUSE_LEFT_CLICK; }
};

/**
 * @brief Gets triggered each time the mouse middle clicks on the parent widget area.
 */
class NEO2D_EXPORT MouseMiddleClickEvent : public Event
{
public:
	MouseMiddleClickEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if(input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_MIDDLE) && getReceiver()->contains(mouse.getPosition()))
			handle();
		else
			reject();
	}

	virtual unsigned int getType()  const { return MOUSE_MIDDLE_CLICK; }
};

/**
 * @brief Gets triggered each time the mouse right clicks on the parent widget area.
 */
class NEO2D_EXPORT MouseRightClickEvent : public Event
{
public:
	MouseRightClickEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if(input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_RIGHT) && getReceiver()->contains(mouse.getPosition()))
			handle();
		else
			reject();
	}

	virtual unsigned int getType() const { return MOUSE_RIGHT_CLICK; }
};

/**
 * @brief Gets triggered each time the left mouse button gets release while being on the parent widget area.
 */
class NEO2D_EXPORT MouseLeftReleaseEvent : public Event
{
public:
	MouseLeftReleaseEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if (input->getMouse().onKeyUp(Neo::MOUSE_BUTTON_LEFT) && getReceiver()->contains(mouse.getPosition()))
			handle();
		else
			reject();
	}

	virtual unsigned int getType() const { return MOUSE_LEFT_RELEASE; }
};

/**
 * @brief Gets triggered each time the middle mouse button gets release while being on the parent widget area.
 */
class NEO2D_EXPORT MouseMiddleReleaseEvent : public Event
{
public:
	MouseMiddleReleaseEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if (input->getMouse().onKeyUp(Neo::MOUSE_BUTTON_MIDDLE) && getReceiver()->contains(mouse.getPosition()))
			handle();
		else
			reject();
	}

	virtual unsigned int getType()  const { return MOUSE_MIDDLE_RELEASE; }
};

/**
 * @brief Gets triggered each time the right mouse button gets release while being on the parent widget area.
 */
class NEO2D_EXPORT MouseRightReleaseEvent : public Event
{
public:
	MouseRightReleaseEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
		: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if (input->getMouse().onKeyUp(Neo::MOUSE_BUTTON_RIGHT) && getReceiver()->contains(mouse.getPosition()))
			handle();
		else
			reject();
	}

	virtual unsigned int getType()  const { return MOUSE_RIGHT_RELEASE; }
};

/**
 * @brief Gets triggered each time the mouse moved.
 */
class NEO2D_EXPORT MouseMoveEvent : public Event
{
	Neo::Vector2 delta;
	Neo::Vector2 lastPosition = Neo::Vector2(0,0);
public:
	MouseMoveEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
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
		else reject();
	}

	/**
	 * @brief Retrieves the 2D movement delta.
	 * @return The movement vector.
	 */
	const Neo::Vector2 &getDelta() const
	{
		return delta;
	}

	/**
	 * @brief Sets the 2D movement delta.
	 * @param delta The new delta.
	 */
	void setDelta(const Neo::Vector2& delta)
	{
		MouseMoveEvent::delta = delta;
	}

	/**
	 * @brief Retrieves the last known position of the cursor.
	 * @return The 2D position.
	 */
	const Neo::Vector2 &getLastPosition() const
	{
		return lastPosition;
	}

	virtual unsigned int getType() const override { return MOUSE_MOVE; }
};

/**
 * @brief Gets triggered each time a key gets pressed.
 */
class NEO2D_EXPORT KeyPressEvent: public Event
{
	unsigned int m_key;
public:
	KeyPressEvent(std::weak_ptr<Neo2D::Gui::Widget> w, const function<void(Widget&, const Event &, void *)>& cb, void* d) :
		Event(w, cb, d),
		m_key(-1)
	{}

	virtual void update(float dt)
	{
		reject();

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Keyboard& kbd = input->getKeyboard();

		for(unsigned int i = 0; i < kbd.getKeys().size(); i++)
		{
			if(kbd.onKeyDown(i))
			{
				m_key = i;
				handle();
			}
		}
	}

	/**
	 * @brief Retrieves the key ID.
	 * @return The key ID.
	 */
	unsigned int getKey() const { return m_key; }
	void setKey(unsigned int k) { m_key = k; }
	virtual unsigned int getType() const { return KEY_PRESSED; }
};


/**
 * @brief Gets triggered each time a key gets released.
 */
class NEO2D_EXPORT KeyReleaseEvent: public Event
{
	unsigned int m_key;
public:
	KeyReleaseEvent(std::weak_ptr<Neo2D::Gui::Widget> w, const function<void(Widget&, const Event &, void *)>& cb, void* d) :
	Event(w, cb, d),
	m_key(-1)
	{}

	virtual void update(float dt)
	{
		reject();

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Keyboard& kbd = input->getKeyboard();

		for(unsigned int i = 0; i < kbd.getKeys().size(); i++)
		{
			if(kbd.onKeyUp(i))
			{
				m_key = i;
				handle();
			}
		}
	}

	/**
	 * @brief Retrieves the key ID.
	 * @return The key ID.
	 */
	unsigned int getKey() const { return m_key; }
	virtual unsigned int getType() const { return KEY_RELEASED; }
};


/**
 * @brief Gets triggered each time an UTF8 character is entered.
 */
class NEO2D_EXPORT CharacterInputEvent: public Event
{
	unsigned int m_key;
public:
	CharacterInputEvent(std::weak_ptr<Neo2D::Gui::Widget> w, const function<void(Widget&, const Event &, void *)>& cb, void* d) :
	Event(w, cb, d),
	m_key(EOF)
	{}

	virtual void update(float dt)
	{
		reject();

		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Keyboard& kbd = input->getKeyboard();

		m_key = kbd.getCharacter();

		if(m_key == EOF)
		{
			reject();
			return;
		}

		handle();

		// Reset last typed character
		if(handled())
			kbd.setCharacter(EOF);
	}

	/**
	 * @brief Retrieves the character.
	 * @return The UTF-8 character.
	 */
	unsigned int getCharacter() const { return m_key; }
	virtual unsigned int getType() const { return CHARACTER_INPUT; }
};

/**
 * @brief Gets triggered each time a mouse click happens outside the parent widget area.
 */
class NEO2D_EXPORT MouseDeselectEvent : public Event
{
public:
	MouseDeselectEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
	: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();

		if((input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_LEFT)
			|| input->getMouse().onKeyDown(Neo::MOUSE_BUTTON_RIGHT))
			&& !getReceiver()->contains(mouse.getPosition()))
				handle();

		// Everything should get this event
		reject();
	}

	virtual unsigned int getType()  const { return MOUSE_DESELECT; }
};

/**
 * @brief Gets triggered each time the mouse wheel is moved.
 */
class NEO2D_EXPORT MouseMiddleMoveEvent : public Event
{
	float delta;
	float value;
public:
	MouseMiddleMoveEvent(std::weak_ptr<Neo2D::Gui::Widget> p, std::function<void(Widget&, const Event&, void*)> cb, void* d)
	: Event(p, cb, d) {}

	virtual void update(float dt)
	{
		Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
		Neo::InputContext* input = engine->getInputContext();
		Neo::Mouse& mouse = input->getMouse();
		float curval = mouse.getScrollValue();

		if(curval != value)
		{
			delta = curval - value;
			value = curval;
			handle();
		}
		else reject();
	}

	float getValue() const
	{
		return value;
	}

	void setValue(float value)
	{
		MouseMiddleMoveEvent::value = value;
	}

	float getDelta() const
	{
		return delta;
	}

	void setDelta(float delta)
	{
		MouseMiddleMoveEvent::delta = delta;
	}

	virtual unsigned int getType() const override { return MOUSE_SCROLL; }
};

}
}

#endif
