#ifndef __LAYOUTSTRATEGY_H
#define __LAYOUTSTRATEGY_H

#include <vector>
#include <memory>
#include "Widget.h"

namespace Neo2D
{
namespace Gui
{
class Container;
class NEO2D_EXPORT LayoutStrategy
{
public:
	virtual void updateLayout(std::vector<shared_ptr<Widget>>& v) = 0;
};

}
}

#endif
