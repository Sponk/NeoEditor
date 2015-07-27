#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H

#include <Neo2DEngine.h>
#include <Container.h>
#include <ScrollBar.h>
#include <vector>

#define m_scrollWidth 1

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
	
	public:
		ScrollPane(unsigned int x, unsigned int y, unsigned int width,
			   unsigned int height)
			: Container(x, y, width, height, "")
		{
			m_horizontal = new ScrollBar(x, y+height-m_scrollWidth,
									  width, m_scrollWidth, 0, 100, SLIDER_HORIZONTAL);
			
			m_vertical = new ScrollBar(x + width, y, m_scrollWidth, height, 0, 100, SLIDER_VERTICAL);
		}

		~ScrollPane();

		void draw() { draw(Vector2(0,0)); }
		void draw(Vector2 offset);
		void update();

 private:
		Vector2 calculateValue() { return getPosition() - Vector2(floor(m_horizontal->getValue()), floor(m_vertical->getValue())); }

		Vector2 calculateContentSize();
};

}
}

#endif
