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

/**
 * @brief Defines the abstract interface for a layout.
 */
class NEO2D_EXPORT LayoutStrategy
{
public:
	/**
	 * @brief Updates the layout of the widgets in a vector.
	 *
	 * This method iterates over each widget in the given vector and
	 * changes their position and size with a layout specific method.
	 *
	 * @param v The vector with widgets.
	 * @param w The parent widget.
	 */
	virtual void updateLayout(const Widget& w, std::vector<shared_ptr<Widget>>& v, const Neo::Vector2& offset) = 0;
};

}
}

#endif
