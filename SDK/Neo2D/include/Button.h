#ifndef __BUTTON_H
#define __BUTTON_H

#include "Widget.h"

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a traditional push button.
 */
class NEO2D_EXPORT Button : public Widget
{
public:
	Button(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr);
	virtual bool handle(const Event& event);

	virtual void init() override;
};

}
}

#endif
