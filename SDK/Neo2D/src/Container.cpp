#include <Container.h>

Neo::Vector2 Neo2D::Gui::Container::calculateContentSize(const Neo::Vector2& offset) const
{
	Neo::Vector2 maxpos;

	for(auto c : m_children)
	{
		Neo::Vector2 pos = c->getPosition() + offset;
		const Neo::Vector2 sz = c->getSize();

		maxpos.x = MAX(maxpos.x, pos.x + sz.x);
		maxpos.y = MAX(maxpos.y, pos.y + sz.y);
	}

	return maxpos - getPosition();
}