#include <NeoEngine.h>
#include <ScaleLayout.h>

using namespace Neo;
using namespace Neo2D;
using namespace Neo2D::Gui;

void ScaleLayout::update()
{
	Widget* parent = getParent();
	if(!parent) return;

	Vector2 parentSize = parent->getSize();
	for (int i = 0; i < m_content.size(); i++)
	{
		Widget* w = m_content[i];

		if(w->isVisible())
		{
			w->setSize(parentSize.x,parentSize.y);
			w->setOffset(m_offset + getPosition());
			w->update();
		}
	}
}
