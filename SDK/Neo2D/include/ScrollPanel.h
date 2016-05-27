#ifndef NEO_SCROLLPANEL_H
#define NEO_SCROLLPANEL_H

#include <Container.h>
#include <Scrollbar.h>
#include "CommonEvents.h"

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a panel with two ScrollBar's.
 */
class NEO2D_EXPORT ScrollPanel : public Container
{
	Scrollbar verticalScroll, horizontalScroll;
	Neo::Vector2 lastOffset;

	void updateScrollbarPlacement();

public:
	ScrollPanel(int x,
				int y,
				unsigned int w,
				unsigned int h,
				const shared_ptr<Object2D>& parent,
				const shared_ptr<Theme>& knobtheme = nullptr,
				const shared_ptr<Theme>& background = nullptr);

	virtual void init() override;
	virtual void draw(const Neo::Vector2& offset);
	virtual void update(float dt);

	virtual bool handle(const Event& e) override
	{
		switch(e.getType())
		{
			case MOUSE_OVER:
				setState(WIDGET_HOVER);
				for(auto c : getChildren())
					c->setActive(true);
				return true;

			case MOUSE_LEAVE:
				setState(WIDGET_NORMAL);
				for(auto c : getChildren())
					c->setActive(false);
				return true;
		}

		return false;
	}
};

}
}

#endif //NEO_SCROLLPANEL_H
