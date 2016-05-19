#ifndef __VERTICAL_LAYOUT_H
#define __VERTICAL_LAYOUT_H

#include <Neo2D.h>
#include <LayoutStrategy.h>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT VerticalLayout : public LayoutStrategy
{
	bool m_resize;
	float m_padding;
public:

	VerticalLayout() : m_resize(true), m_padding(0) {}
	virtual void updateLayout(const Widget& w, std::vector<shared_ptr<Widget>>& v, const Neo::Vector2& offset);

	bool isResizing() const { return m_resize; }
	void enableResize(bool v) { m_resize = v; }

	float getPadding() const { return m_padding; }
	void setPadding(float p) { m_padding = p; }
};
	
}
}

#endif
