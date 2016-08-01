#ifndef __WIDGET_H
#define __WIDGET_H

#include "Neo2D.h"
#include "Object2D.h"
#include "Event.h"
#include "Theme.h"

#include <NeoEngine.h>

namespace Neo2D
{
namespace Gui
{

/**
 * Encodes the states a widget might have.
 * These are used by widget logic and the visual themes mainly.
 */
enum WIDGET_STATE
{
	/// The Widgets is in its normal default state
	WIDGET_NORMAL,
	/// The mouse cursor is hovering over the widget
	WIDGET_HOVER,
	/// The Widget is selected
	WIDGET_SELECTED,
	/// The Widget is being triggered
	WIDGET_TRIGGERED
};

/**
 * @brief Implements an empty widget without any logic regarding the use as a UI element.
 *
 * This class implements basic functionality that is useful for any type of Widget like callbacks,
 * events, theme handling and similar. It provides mechanisms for parent/child relationships but
 * requires children to be dynamically allocated i.e. created using make_shared since the implementation
 * works using weak_ptr's.
 *
 * @see Container
 */
class NEO2D_EXPORT Widget : public Object2D, public enable_shared_from_this<Widget>
{
#ifndef SWIG
	std::vector<shared_ptr<Event>> m_events;
#endif

	shared_ptr<Theme> m_theme;
	WIDGET_STATE m_state;

	// A string to reference a script function name
	std::string m_scriptCallback;
	std::function<void(Widget&, void*)> m_callback;
	void* m_data;

	std::string m_label;
	bool m_initialized;

protected:
	bool isInitialized() const { return m_initialized; }

public:
	Widget(int x, int y, unsigned int w, unsigned int h, const char* label,
		   const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr);

	/**
	 * @brief Cleans up by unregistering all events registered by the destroyed instance.
	 */
	virtual ~Widget()
	{
		unregisterEvent(*this);
	}

	virtual void draw(const Neo::Vector2& offset)
	{
		if (m_theme != nullptr && !isInvisible()) m_theme->draw(this, offset);
	}

	/**
	 * @brief Initializes the widget and registers all events.
	 */
	virtual void init() {}
	virtual void update(float dt);

	virtual void doInit() { if(!m_initialized) { init(); m_initialized = true; } }

	/**
	 * @brief Handles all registered events.
	 *
	 * This method is called by the Event instance when the event is triggered.
	 * @param e The event that got triggered.
	 * @return A boolean value that indicates whether the event was handled or not.
	 */
	virtual bool handle(const Event& e) { return true; }

	/**
	 * @brief Registers an Event.
	 *
	 * If the Widget has a parent the event is passed up so all events
	 * are handled in one place (the uppermost parent).
	 *
	 * @param e The event to register.
	 */
	void registerEvent(shared_ptr<Event> e)
	{
		m_events.push_back(e);

		if(auto parent = getParent().lock())
			dynamic_cast<Widget*>(parent.get())->registerEvent(e);
	}

	/**
	 * @brief Destroys all events that are registered to the given Widget.
	 *
	 * If the instance has a parent this call is passed up to the uppermost parent
	 * which will then delete all matching events.
	 *
	 * @param w The widget that owns the events.
	 */
	void unregisterEvent(Widget& w);

	/**
	 * @brief Retrieves the Widget state.
	 * @return The state.
	 */
	WIDGET_STATE getState() const { return m_state; }

	/**
	 * @brief Sets the callback which is called when a Widget specific criterium is met.
	 *
	 * @brief cb The callback to call.
	 * @brief data The data that is passed to the callback when it is called.
	 */
	void setCallback(std::function<void(Widget&, void*)> cb, void* data) { m_callback = cb; m_data = data; }

	/**
	 * @brief Retrieves the label of the Widget.
	 * @return The label.
	 */
	const char* getLabel() { return m_label.c_str(); }

	/**
	 * @brief Changes the label.
	 * @param label The new label to use.
	 */
	void setLabel(const char* label) { m_label = label; }

	void setScriptCallback(const char* str) { m_scriptCallback = str; }
	const char* getScriptCallback() { return m_scriptCallback.c_str(); }

protected:

	/**
	 * @brief Change the state of the Widget.
	 * @param s The new state.
	 */
	void setState(WIDGET_STATE s) { m_state = s; }

	/**
	 * @brief Executes the callback if one is set.
	 */
	void doCallback() { if (m_callback) m_callback(*this, m_data); }

	/**
	 * @brief Retrieves the callback that is registered.
	 * @return The callback.
	 */
	std::function<void(Widget&, void*)> getCallback() const { return m_callback; }

	/**
	 * @brief Retrieves the currently used theme.
	 * @return The theme that is being used.
	 */
	const shared_ptr<Theme>& getTheme() const { return m_theme; }

	/**
	 * @brief Changes the theme that is being used.
	 * @param theme The new theme to use.
	 */
	void setTheme(const shared_ptr<Theme>& theme) { m_theme = theme; }
};

}
}

#endif //__WIDGET_H
