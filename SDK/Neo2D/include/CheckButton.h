#ifndef NEO_CHECKBOX_H
#define NEO_CHECKBOX_H

#include <Widget.h>
#include <Label.h>
#include <Button.h>

namespace Neo2D
{
namespace Gui
{
/**
 * @brief Implements a check button that represents a binary state.
 *
 * The button also displays a text label on the right side of the actual button.
 */
class NEO2D_EXPORT CheckButton : public Widget
{
	shared_ptr<Button> m_button;
	shared_ptr<Label> m_label;

	bool m_value;

protected:
	shared_ptr<Button> getButtonWidget() { return m_button; }
	shared_ptr<Label> getLabelWidget() { return m_label; }

public:
	CheckButton(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent);
	void draw(const Neo::Vector2& offset) override;
	void update(float dt) override;

	/**
	 * @brief Changes the current boolean state.
	 * @param b A boolean value.
	 */
	void setValue(const bool b) { m_value = b; }

	/**
	 * @brief Retrieves the current boolean state.
	 * @return A boolean value.
	 */
	bool getValue() const { return m_value; }
};

}
}

#endif //NEO_CHECKBOX_H
