#include <ImageButton.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

ImageButton::ImageButton(int x,
						 int y,
						 unsigned int w,
						 unsigned int h,
						 const char* label,
						 const shared_ptr<Object2D>& parent,
						 const shared_ptr<Theme>& theme)
	: Button(x, y, w, h, nullptr, parent, theme),
	  m_sprite(x,y,w,h,label, nullptr)
{}
