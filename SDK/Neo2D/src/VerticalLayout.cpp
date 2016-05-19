#include <VerticalLayout.h>

void Neo2D::Gui::VerticalLayout::updateLayout(const Widget& w, std::vector<shared_ptr<Widget>>& v, const Neo::Vector2& offset)
{
	float sx = w.getSize().x, sy = w.getSize().y / v.size();
	float py = w.getPosition().y + m_padding;
	for(auto c : v)
	{
		if(m_resize)
			c->setSize(Neo::Vector2(sx, sy));

		c->setPosition(offset + Neo::Vector2(w.getPosition().x, py));
		py += (m_padding + ((m_resize) ? sy : c->getSize().y));
	}
}

