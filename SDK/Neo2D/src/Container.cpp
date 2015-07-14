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
		w->draw();
}

void Container::update()
{
	for(Widget* w : m_content)
	{
		w->setOffset(getPosition());
		w->update();
	}
}
