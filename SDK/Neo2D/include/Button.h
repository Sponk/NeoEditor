#ifndef __BUTTON_H
#define __BUTTON_H

#include "Widget.h"

namespace Neo2D
{
namespace Gui
{
	
class NEO2D_EXPORT Button : public Widget
{
public:
	Button(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr);
	virtual void handle(const Event& event);
};

}
}

#endif