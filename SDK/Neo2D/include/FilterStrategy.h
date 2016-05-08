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

/**
 * @brief Defines the abstract interface for a filter.
 */
class NEO2D_EXPORT FilterStrategy
{
public:
	/**
	 * @brief Updates the visibility of a vector of widgets.
	 *
	 * Iterates over the given vector and decides for each widget in it if the widget should be visible or not.
	 * @param v The vector containing the widgets.
	 */
	virtual void updateVisibility(std::vector<shared_ptr<Widget>>& v) = 0;
};

}
}

#endif
