#ifndef NEO_SIDEPANEL_H
#define NEO_SIDEPANEL_H

#include <Container.h>

enum WINDOW_EDGE
{
	LEFT_EDGE,
	RIGHT_EDGE,
	TOP_EDGE,
	BOTTOM_EDGE
};

class ScaleLayout : public Neo2D::Gui::LayoutStrategy
{
public:
	virtual void updateLayout(const Neo2D::Gui::Widget& wdg, std::vector<shared_ptr<Neo2D::Gui::Widget>>& v, const Neo::Vector2& offset) override
	{
		for(auto w : v)
		{
			w->setPosition(wdg.getPosition());
			w->setSize(wdg.getSize());
		}
	}
};

class Sidepanel : public Neo2D::Gui::Container
{
	WINDOW_EDGE edge;
public:
	Sidepanel(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Neo2D::Object2D>& parent)
		: Container(x, y, w, h, parent)
	{ }

	virtual void update(float dt);

	WINDOW_EDGE getEdge() const
	{
		return edge;
	}

	void setEdge(WINDOW_EDGE edge)
	{
		Sidepanel::edge = edge;
	}
};


#endif //NEO_SIDEPANEL_H
