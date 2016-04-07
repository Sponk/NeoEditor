#ifndef __THEME_H
#define __THEME_H

#include <Neo2D.h>
#include <NeoCore.h>

namespace Neo2D
{

namespace Gui
{
class Widget;

class NEO2D_EXPORT Theme
{
public:
	virtual void draw(Gui::Widget* widget, const Neo::Vector2& offset) = 0;
};

}
}

#endif