#include <Event.h>
#include <Widget.h>

void Neo2D::Gui::Event::handle(Neo2D::Gui::Widget& w)
{
	w.handle(*this);
}