#include "ColorEdit.h"

ColorEdit::ColorEdit(int x,
					 int y,
					 unsigned int w,
					 unsigned int h,
					 const char* label,
					 const shared_ptr<Neo2D::Object2D>& parent,
					 const shared_ptr<NativeToolset>& tools,
					 const shared_ptr<Neo2D::Gui::Theme>& theme) :
	Neo2D::Gui::Widget(x, y, w, h, label, parent),
	m_button(make_shared<Neo2D::Gui::Button>(x,y,w,h, label, parent, theme)),
	m_toolset(tools)
{
	m_button->setCallback([this](Neo2D::Gui::Widget&, void*) {
		m_color = m_toolset->colorDialog(m_color);
		doCallback();
	}, nullptr);
}

void ColorEdit::update(float dt)
{
	m_button->setPosition(getPosition());
	m_button->setSize(getSize());

	m_button->update(dt);
	Neo2D::Gui::Widget::update(dt);
}

void ColorEdit::draw(const Neo::Vector2& offset)
{
	m_button->draw(offset);

	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	Neo::Renderer* render = engine->getRenderer();

	render->drawColoredQuad(getPosition() + Neo::Vector2(3,3), getSize() - Neo::Vector2(6,6), m_color, 0);
}
