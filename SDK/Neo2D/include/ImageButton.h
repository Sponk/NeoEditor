#ifndef NEO_IMAGEBUTTON_H
#define NEO_IMAGEBUTTON_H

#include <Button.h>
#include <Sprite.h>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a traditional push button with an icon instead of text.
 * @see Button
 */
class ImageButton : public Button
{
	Neo::Vector2 m_padding;
	Sprite m_sprite;
public:
	ImageButton(int x,
				int y,
				unsigned int w,
				unsigned int h,
				const char* image,
				const shared_ptr<Object2D>& parent,
				const shared_ptr<Theme>& theme = nullptr);

	virtual void draw(const Neo::Vector2& offset) override
	{
		Widget::draw(offset);
		
		m_sprite.setPosition(getPosition() + m_padding);
		m_sprite.draw(offset);
	}
};

}
}

#endif //NEO_IMAGEBUTTON_H
