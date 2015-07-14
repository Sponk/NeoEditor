#ifndef __WINDOW_H
#define __WINDOW_H

#include <Neo2DEngine.h>
#include <Container.h>
#include <vector>

namespace Neo2D
{
namespace Gui
{
using namespace Neo;

/**
* Represents the state a window can possibly have.
*/
enum WINDOW_STATE
{
	WINDOW_UNSELECTED_STATE = 0,
	WINDOW_GRABBED_STATE,
	WINDOW_SELECTED_STATE,
	WINDOW_RESIZING_STATE
};

/**
 * @brief This class implements a Window that can be used to display widgets.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Window : public Container
{

	protected:
		OText* m_labelText;

		float m_mx;
		float m_my;

		WINDOW_STATE m_state;
		
	public:
			Window(unsigned int x, unsigned int y, unsigned int width,
			unsigned int height, const char* label)
			: Container(x, y, width, height, label),
			m_labelText(NULL),
			m_state(WINDOW_SELECTED_STATE)
			{
			}
				
        ~Window();

		void draw();
		void draw(Vector2 offset) { draw(); }
		void update();
};

}
}

#endif
