#ifndef NEO_TOOLBAR_H
#define NEO_TOOLBAR_H

#include <Container.h>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a toolbar which can contain ImageButton's.
 */
class NEO2D_EXPORT Toolbar : public Container
{

public:
	Toolbar(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme = nullptr);

	virtual void draw(const Neo::Vector2& offset) override
	{
		getTheme()->draw(this, offset);
		Container::draw(offset);
	}
};

}
}

#endif //NEO_TOOLBAR_H
