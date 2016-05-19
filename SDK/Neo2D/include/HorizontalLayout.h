#ifndef __HORIZONTAL_LAYOUT_H
#define __HORIZONTAL_LAYOUT_H

#include <Neo2D.h>
#include <LayoutStrategy.h>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT HorizontalLayout : public LayoutStrategy
{
public:
	virtual void updateLayout(const Widget& w, std::vector<shared_ptr<Widget>>& v, const Neo::Vector2& offset);
};
	
}
}

#endif
