#ifndef __WIDGET_H
#define __WIDGET_H

#include "Neo2D.h"
#include "Object2D.h"
#include "Event.h"
#include "Theme.h"
#include <memory>

#include <NeoEngine.h>

namespace Neo2D
{
namespace Gui
{

enum WIDGET_STATE
{
	WIDGET_NORMAL,
	WIDGET_HOVER,
	WIDGET_SELECTED,
	WIDGET_TRIGGERED
};

class NEO2D_EXPORT Widget : public Object2D
{
	std::vector<std::shared_ptr<Event>> m_events;
	std::shared_ptr<Theme> m_theme;
	WIDGET_STATE m_state;

	std::function<void(Widget&, void*)> m_callback;
	void* m_data;

	Neo::String m_label;

public:
	Widget(int x, int y, unsigned int w, unsigned int h, const char* label,
		   const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr);
	virtual ~Widget() {}

	virtual void draw(const Neo::Vector2& offset)
	{
		if (m_theme != nullptr) m_theme->draw(this, offset);
	}

	virtual void update(float dt);
	virtual void handle(const Event& e) {}
	
	void registerEvent(shared_ptr<Event> e)
	{
		m_events.push_back(e);

		if(auto parent = getParent().lock())
			dynamic_cast<Widget*>(parent.get())->registerEvent(e);
	}

	WIDGET_STATE getState() const { return m_state; }
	void setCallback(std::function<void(Widget&, void*)> cb, void* data) { m_callback = cb; m_data = data; }

	const char* getLabel() { return m_label.getSafeString(); }
	void setLabel(const char* label) { m_label.set(label); }

protected:
	void setState(WIDGET_STATE s) { m_state = s; }
	void doCallback() { if (m_callback) m_callback(*this, m_data); }

	const std::shared_ptr<Theme>& getTheme() const { return m_theme; }
	void setTheme(const shared_ptr<Theme>& theme) { m_theme = theme; }
};

}
}

#endif //__WIDGET_H
