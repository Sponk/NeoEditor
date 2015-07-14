#ifndef __WINDOW_MANAGER_H
#define __WINDOW_MANAGER_H

#include <Neo2DEngine.h>
#include <Container.h>
#include <vector>

namespace Neo2D
{
namespace Gui
{
using namespace Neo;

// Forward declaration of Gui::Window
class Window;
	
/**
 * @brief This class manages all windows on screen, that means managing
 * order and selection of windows.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT WindowManager : public Container
{
 private:
	Window* m_selectedWindow;
		
 public:
	WindowManager(unsigned int x, unsigned int y, unsigned int width,
			  unsigned int height, const char* label)
		: Container(x, y, width, height, label),
		m_selectedWindow(NULL)
		{
		}
	
	~WindowManager();

	void addWindow(Window* window);	
	void draw();
	void update();

	void selectWindow(Window* w);
	Window* getSelectedWindow() { return m_selectedWindow; }
};

}
}

#endif
