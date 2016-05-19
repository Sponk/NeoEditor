#include <Event.h>
#include <Widget.h>

void Neo2D::Gui::Event::handle()
{
	if(m_callback != nullptr) m_callback(m_receiver, *this, m_data);
	m_handled = m_receiver.handle(*this);
}
