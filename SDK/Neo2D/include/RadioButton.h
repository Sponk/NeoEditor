#ifndef NEO_RADIOBUTTON_H
#define NEO_RADIOBUTTON_H

#include <Widget.h>
#include <CheckButton.h>
#include <Container.h>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT RadioButton : public CheckButton
{
public:
	RadioButton(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent);
};

class NEO2D_EXPORT RadioGroup : public Container
{
public:
	RadioGroup(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent)
		: Container(x, y, w, h, parent)
	{}

	/**
	 * @brief Sets value of all children to false while ignoring children which are not RadioButton's.
	 */
	void reset()
	{
		for(auto p : getChildren())
		{
			RadioButton* btn = dynamic_cast<RadioButton*>(p.get());
			if(btn)
				btn->setValue(false);
		}
	}
};

}
}

#endif //NEO_RADIOBUTTON_H
