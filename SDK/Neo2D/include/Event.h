#ifndef __EVENT_H
#define __EVENT_H

#include <functional>
#include <Neo2D.h>

namespace Neo2D
{
namespace Gui
{
class Widget;
class NEO2D_EXPORT Event
{
public:
	virtual void update(Widget& w, float dt) = 0;
	virtual void handle(Widget& w);
	virtual unsigned int getType() const { return 0; }
};

}
}

#endif //__EVENT_H
