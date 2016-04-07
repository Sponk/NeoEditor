#include "Widget.h"

using namespace Neo2D::Gui;

Widget::Widget(int x, int y, unsigned int w, unsigned int h, 
				const char* label,
				std::shared_ptr<Object2D> parent,
				std::shared_ptr<Theme> theme) :
	Object2D(x, y, w, h, parent),
	m_theme(theme),
	m_state(WIDGET_NORMAL),
	m_label(label)
{
	
}