#ifndef __CONTAINER_H
#define __CONTAINER_H

#include <Neo2DEngine.h>
#include <Widget.h>
#include <vector>

namespace Neo2D
{
namespace Gui
{
using namespace Neo;

/**
 * @brief A Container organizes multiple Widgets into one to provide
 * layout functionality.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Container : public Widget
{
	protected:
		std::vector<Widget*> m_content;

		public:
		Container(unsigned int x, unsigned int y, unsigned int width,
			unsigned int height, const char* label)
			: Widget(x, y, width, height, label)
			{
			}
				
        ~Container();

		DISOWN(Widget* w) void addWidget(Widget* w)
		{
			w->setParent(this);
			m_content.push_back(w);
		}

		std::vector<Widget*>& getContent() { return m_content; }

		void draw();
		void draw(Vector2 offset) { draw(); }
		void update();
};

}
}

#endif
