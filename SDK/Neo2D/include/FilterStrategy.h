#ifndef __FILTERSTRATEGY_H
#define __FILTERSTRATEGY_H

#include <vector>
#include <memory>
#include "Widget.h"

namespace Neo2D
{
namespace Gui
{

class Container;
class FilterStrategy
{
public:
	virtual void updateVisibility(std::vector<shared_ptr<Widget>>& v) = 0;
};

}
}

#endif
