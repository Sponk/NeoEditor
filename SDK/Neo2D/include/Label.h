#ifndef NEO_LABEL_H
#define NEO_LABEL_H

#include "Widget.h"

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT Label : public Widget
{
public:
	Label(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent);
};

}
}

#endif //NEO_LABEL_H
