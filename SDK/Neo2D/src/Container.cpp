#include <NeoEngine.h>
#include <Container.h>

using namespace Neo;
using namespace Neo2D;
using namespace Neo2D::Gui;

Container::~Container()
{
	for(Widget* w : m_content)
		SAFE_DELETE(w);

	m_content.clear();
}

void Container::draw()
{
	for(Widget* w : m_content)
	{
		w->setOffset(m_offset + getPosition());
		if(w->isVisible()) w->draw();
	}
}

void Container::update()
{
	for (int i = 0; i < m_content.size(); i++)
	{
		Widget* w = m_content[i];

		if(w->isVisible())
		{
			w->setOffset(m_offset + getPosition());
			w->update();
		}
	}
}

