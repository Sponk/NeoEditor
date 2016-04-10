#ifndef NEO_CHECKBOX_H
#define NEO_CHECKBOX_H

#include <Widget.h>
#include <Label.h>
#include <Button.h>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT CheckButton : public Widget
{
	Button m_button;
	Label m_label;

	bool m_value;

protected:
	Button& getButtonWidget() { return m_button; }
	Label& getLabelWidget() { return m_label; }

public:
	CheckButton(int x, int y, unsigned int w, unsigned int h, const char* label, shared_ptr<Object2D> parent);
	void draw(const Neo::Vector2& offset) override;
	void update(float dt) override;

	void setValue(const bool b) { m_value = b; }
	bool getValue() const { return m_value; }
};

}
}

#endif //NEO_CHECKBOX_H
