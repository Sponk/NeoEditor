#include <Event.h>
#include <Widget.h>

void Neo2D::Gui::Event::handle()
{
	if(m_receiver.expired())
		return;

	if(m_callback != nullptr) m_callback(*m_receiver.lock(), *this, m_data);
	m_handled = m_receiver.lock()->handle(*this);
}
