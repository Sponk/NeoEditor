#include <CheckButton.h>

Neo2D::Gui::CheckButton::CheckButton(int x, int y, unsigned int w, unsigned int h,
						   const char* label, shared_ptr<Neo2D::Object2D> parent) :
	Neo2D::Gui::Widget(x, y, w, h, label, parent, nullptr),
	m_button(x, y, h, h, "", nullptr),
	m_label(x + h, y, w - h, h, label, nullptr),
	m_value(false)
{
	m_button.setCallback(
	[](Widget& widget, void* data)
	{
		CheckButton* self = reinterpret_cast<CheckButton*>(data);
		self->setValue(!self->getValue());
	}, this);
}

void Neo2D::Gui::CheckButton::draw(const Neo::Vector2& offset)
{
	if(getValue())
		m_button.setLabel("x");
	else
		m_button.setLabel("");

	m_button.draw(offset);

	m_label.setLabel(getLabel());
	m_label.draw(offset + Neo::Vector2(3, getSize().y * 0.25));
}

void Neo2D::Gui::CheckButton::update(float dt)
{
	m_button.update(dt);
}