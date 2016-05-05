#ifndef __CONTAINER_H
#define __CONTAINER_H

#include "Neo2D.h"
#include "Widget.h"
#include "FilterStrategy.h"
#include "LayoutStrategy.h"

#include <vector>
#include <memory>
#include <algorithm>

namespace Neo2D
{
namespace Gui
{
class NEO2D_EXPORT Container: public Widget, public std::enable_shared_from_this<Container>
{
private:
	std::vector<shared_ptr<Widget>> m_children;
	shared_ptr<LayoutStrategy> m_layout;
	shared_ptr<FilterStrategy> m_filter;

protected:
	std::vector<shared_ptr<Widget>>& getChildren() { return m_children; }

public:
	Container(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent)
		: Widget(x, y, w, h, nullptr, parent), m_layout(nullptr), m_filter(nullptr)
	{ }

	size_t addWidget(const shared_ptr<Widget>& o)
	{
		m_children.push_back(o);
		o->setParent(shared_from_this());
		return m_children.size() - 1;
	}

	void removeWidget(const shared_ptr<Widget>& o)
	{ m_children.erase(std::find(m_children.begin(), m_children.end(), o)); }

	void removeWidget(size_t id)
	{
		if (id >= m_children.size()) return;
		m_children.erase(m_children.begin() + id);
	}

	shared_ptr<Widget> getWidget(size_t id) { if(id >= m_children.size()) return nullptr; return m_children[id]; }
	size_t getWidgetCount() { return m_children.size(); }

	Neo::Vector2 calculateContentSize(const Neo::Vector2& offset = Neo::Vector2()) const;

	void clear()
	{ m_children.clear(); }

	virtual void draw(const Neo::Vector2 &offset)
	{
		for (auto o : m_children)
			o->draw(offset);
	}

	virtual void update(float dt)
	{
		for (auto o : m_children)
			o->update(dt);

		Widget::update(dt);
	}

	void updateFilter() { if(m_filter != nullptr) m_filter->updateVisibility(m_children); }
	void updateLayout() { if(m_layout != nullptr) m_layout->updateLayout(m_children); }

	void setFilter(const shared_ptr<FilterStrategy> &m_filter)
	{
		Container::m_filter = shared_ptr<FilterStrategy>(m_filter);
	}

	void setLayout(const shared_ptr<LayoutStrategy> &m_layout)
	{
		Container::m_layout = shared_ptr<LayoutStrategy>(m_layout);
	}

	virtual void handle(Event& e)
	{
		for(auto w : m_children)
			w->handle(e);
	}
};
}
}

#endif