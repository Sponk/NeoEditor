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

	virtual void update(float dt) override
	{
		Neo::Vector2 size(getSize().y, getSize().y);
		float px = getPosition().x;
		for(auto c : getChildren())
		{
			c->setSize(size);
			c->setPosition(Neo::Vector2(px, getPosition().y));
			c->update(dt);
			px += size.x;			
		}
	}
};

}
}

#endif //NEO_TOOLBAR_H
