#include <ImageButton.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

#define PADDING 0.05

ImageButton::ImageButton(int x,
			 int y,
			 unsigned int w,
			 unsigned int h,
			 const char* label,
			 const shared_ptr<Object2D>& parent,
			 const shared_ptr<Theme>& theme)
	: Button(x, y, w, h, nullptr, parent, theme),
	  // Leave some edge space
	  m_padding(PADDING*w, PADDING*h),
	  m_sprite(x+m_padding.x,y+m_padding.y,w-m_padding.x*2.0f,h-m_padding.y*2.0f,label, nullptr)
{}

void ImageButton::loadImage(const char* path)
{
	m_sprite = Sprite(getPosition().x + m_padding.x, 
			  getPosition().y + m_padding.y,
			  getSize().x - m_padding.x * 2.0f, 
			  getSize().y - m_padding.y * 2.0f,
			  path,
			  nullptr);
}
