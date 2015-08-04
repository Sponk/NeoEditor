#ifndef __SCALE_LAYOUT_H
#define __SCALE_LAYOUT_H

#include <Neo2DEngine.h>
#include <Container.h>
#include <vector>

namespace Neo2D
{
namespace Gui
{
using namespace Neo;

/**
 * @brief Scales the containing widget to the size of its parent.
 *
 * Attention: Should only contain one child!
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT ScaleLayout : public Container
{
		public:
		ScaleLayout(unsigned int x, unsigned int y, unsigned int width,
			unsigned int height)
			: Container(x, y, width, height, "")
			{
			}
				
		void update();
};

}
}

#endif
