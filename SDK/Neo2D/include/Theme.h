#ifndef __THEME_H
#define __THEME_H

#include <Neo2D.h>
#include <NeoCore.h>

namespace Neo2D
{

namespace Gui
{
class Widget;

/**
 * @brief Defines the abstract interface for a theme object.
 */
class NEO2D_EXPORT Theme
{
public:
	/**
	 * @brief Draws a widget on the screen.
	 * @param widget The widget to draw.
	 * @param offset The offset to apply to the widget position.
	 */
	virtual void draw(Gui::Widget* widget, const Neo::Vector2& offset) = 0;
};

}
}

#endif