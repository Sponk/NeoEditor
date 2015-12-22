#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H

#include <Neo2DEngine.h>
#include <Container.h>
#include <ScrollBar.h>
#include <vector>

#define m_scrollWidth 1
#define SCROLL_BAR_WIDTH 10

namespace Neo2D
{
namespace Gui
{
using namespace Neo;

/**
 * @brief This class implements a scroll panel that can be used to display widgets.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT ScrollPane : public Container
{
	ScrollBar* m_horizontal;
	ScrollBar* m_vertical;

	void positionScrollBars()
	{		
		// Update scale and position
		m_horizontal->setSize(m_width - SCROLL_BAR_WIDTH, 1);
		m_vertical->setSize(1, m_height - SCROLL_BAR_WIDTH);

		m_horizontal->setPosition(Vector2(m_x, m_y+m_height-SCROLL_BAR_WIDTH));
		m_vertical->setPosition(Vector2(m_x + m_width - SCROLL_BAR_WIDTH, m_y));

		Vector2 sz = calculateContentSize();
		m_horizontal->setRange(Vector2(0, MAX(m_width - SCROLL_BAR_WIDTH, sz.x)));
		m_vertical->setRange(Vector2(0, MAX(m_height - SCROLL_BAR_WIDTH, sz.y)));
	}
	
public:
	ScrollPane(unsigned int x, unsigned int y, unsigned int width,
		   unsigned int height)
		: Container(x, y, width, height, "")
	{
		m_horizontal = new ScrollBar(x, y+height-m_scrollWidth, width, m_scrollWidth, 0, 100, SLIDER_HORIZONTAL);
		
		m_vertical = new ScrollBar(x + width, y, m_scrollWidth, height, 0, 100, SLIDER_VERTICAL);
		positionScrollBars();
	}

	~ScrollPane();

	void draw() { draw(Vector2(0,0)); }
	void draw(Vector2 offset);
	void update();

	Vector2 getSize() { return Vector2(m_width - SCROLL_BAR_WIDTH, m_height - SCROLL_BAR_WIDTH); }

 private:
	Vector2 calculateValue() { return getPosition() - Vector2(floor(m_horizontal->getValue()), floor(m_vertical->getValue())); }
	Vector2 calculateContentSize();
};

}
}

#endif
