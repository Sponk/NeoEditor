#include <CheckButton.h>

Neo2D::Gui::CheckButton::CheckButton(int x, int y, unsigned int w, unsigned int h,
						   const char* label, const shared_ptr<Neo2D::Object2D>& parent) :
	Neo2D::Gui::Widget(x, y, w, h, label, parent, nullptr),
	m_button(x, y, h, h, "", parent),
	m_label(x + h, y, w - h, h, label, parent),
	m_value(false)
{
	m_label.setColor(Neo::Vector4(0,0,0,1));
	m_button.setCallback(
	[](Widget& widget, void* data)
	{
		CheckButton* self = reinterpret_cast<CheckButton*>(data);
		self->setValue(!self->getValue());
		self->doCallback();
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
	m_button.setPosition(getPosition());
	m_label.setPosition(Neo::Vector2(getPosition().x + getSize().y, getPosition().y));
	m_button.update(dt);
}
