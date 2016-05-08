#ifndef NEO_RADIOBUTTON_H
#define NEO_RADIOBUTTON_H

#include <Widget.h>
#include <CheckButton.h>
#include <Container.h>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a radio button.
 *
 * A radio button differs from a check button in that a radio button remains true once it was
 * set to true by clicking.
 */
class NEO2D_EXPORT RadioButton : public CheckButton
{
public:
	RadioButton(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent);
};

/**
 * @brief Implements a group of radio buttons.
 *
 * Only one radio button can be true in one radio group which is useful for implementing mutual exclusive
 * settings.
 */
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
