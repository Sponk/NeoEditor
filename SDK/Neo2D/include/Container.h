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

/**
 * @brief Implements a container that can have multiple children.
 *
 * Children get automatically updated and drawn when the container is updated or drawn.
 * It also delivers an interface to lay the widgets out or to filter them without forcing
 * a certain behavior.
 *
 * Container always have to be created using make_shared since each children gets a weak_ptr to its parent
 * during creation.
 */
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

	/**
	 * @brief Adds a new child to the internal widget list.
	 * @param o The new widget.
	 * @return The widget ID.
	 */
	size_t addWidget(const shared_ptr<Widget>& o)
	{
		m_children.push_back(o);
		o->setParent(shared_from_this());
		return m_children.size() - 1;
	}

	/**
	 * @brief Removes a widget from the internal list.
	 * @param o The widget to remove.
	 */
	void removeWidget(const shared_ptr<Widget>& o)
	{ m_children.erase(std::find(m_children.begin(), m_children.end(), o)); }

	/**
	 * @brief Removes a widget from the internal list.
	 * @param id The widget ID.
	 */
	void removeWidget(size_t id)
	{
		if (id >= m_children.size()) return;
		m_children.erase(m_children.begin() + id);
	}

	/**
	 * @brief Retrieves a widget from the internal list.
	 * @param id The widget ID.
	 * @return The widget with the corresponding ID.
	 */
	shared_ptr<Widget> getWidget(size_t id) { if(id >= m_children.size()) return nullptr; return m_children[id]; }

	/**
	 * @brief Retrieves the number of widgets registered.
	 * @return The number widgets.
	 */
	size_t getWidgetCount() { return m_children.size(); }

	/**
	 * @brief Calculates the size of the container.
	 *
	 * The calculation always includes the origin of the container and takes position and size of all children into
	 * account.
	 *
	 * @param offset A vector used to offset the childrens position.
	 * @return A 2D vector which contains the required (width, height) of the container.
	 */
	Neo::Vector2 calculateContentSize(const Neo::Vector2& offset = Neo::Vector2()) const;

	/**
	 * @brief Clears the internal widget list.
	 */
	void clear()
	{ m_children.clear(); }

	virtual void draw(const Neo::Vector2 &offset)
	{
		for (auto o : m_children)
			o->draw(offset);
	}

	virtual void update(float dt)
	{
		updateFilter();
		for (auto o : m_children)
			o->update(dt);
		updateLayout();

		Widget::update(dt);
	}

	/**
	 * @brief Applies the registered filter.
	 */
	void updateFilter() { if(m_filter != nullptr) m_filter->updateVisibility(m_children); }

	/**
	 * @brief Applies the registered layout.
	 */
	void updateLayout(const Neo::Vector2& offset) { if(m_layout != nullptr) m_layout->updateLayout(*this, m_children, offset); }
	void updateLayout() { updateLayout(Neo::Vector2()); }

	/**
	 * @brief Registers a new filter.
	 * @param m_filter The new filter to use.
	 */
	void setFilter(const shared_ptr<FilterStrategy> &m_filter)
	{
		Container::m_filter = shared_ptr<FilterStrategy>(m_filter);
	}

	/**
	 * @brief Registers a new layout.
	 * @param m_layout The new layout to use.
	 */
	void setLayout(const shared_ptr<LayoutStrategy> &m_layout)
	{
		Container::m_layout = shared_ptr<LayoutStrategy>(m_layout);
	}

	const shared_ptr<LayoutStrategy>& getLayout() const { return m_layout; }

	virtual bool handle(Event& e)
	{
		bool handled = false;
		for(auto w : m_children)
			handled |= w->handle(e);

		return handled;
	}
};
}
}

#endif
