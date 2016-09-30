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
class NEO2D_EXPORT ImageButton : public Button
{
private:
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

	const Neo::Vector2& getPadding() const
	{
		return m_padding;
	}

	void setPadding(const Neo::Vector2& padding)
	{
		m_padding = padding;
	}
	
	/**
	 * @brief Loads a new image from the given path and displays it.
	 * 
	 * Overwrites the current image and might cause disk access.
	 * 
	 * @param path The new image to use.
	 */
	void loadImage(const char* path);
};

}
}

#endif //NEO_IMAGEBUTTON_H
