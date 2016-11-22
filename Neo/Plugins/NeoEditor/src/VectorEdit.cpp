#include "VectorEdit.h"
// Vector3
Vector3Edit::Vector3Edit(int x,
						 int y,
						 unsigned int w,
						 unsigned int h,
						 const char* label,
						 const shared_ptr<Neo2D::Object2D>& parent,
						 const shared_ptr<Neo2D::Gui::Theme>& theme) :
	m_x(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
	m_y(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
	m_z(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
	VectorEdit(x, y, w, h, label, parent)
{
	m_cb = [this](Neo2D::Gui::Widget&, void*) {
		doCallback();
	};
	
	m_x->setCallback(m_cb, nullptr);
	m_y->setCallback(m_cb, nullptr);
	m_z->setCallback(m_cb, nullptr);
}

void Vector3Edit::updateContent()
{
	float sx = getSize().x / 3;
	m_x->setSize(Neo::Vector2(sx, getSize().y));
	m_y->setSize(Neo::Vector2(sx, getSize().y));
	m_z->setSize(Neo::Vector2(sx, getSize().y));
	
	m_x->setPosition(getPosition());

	// FIXME: Configurable spacing!
	m_y->setPosition(getPosition() + Neo::Vector2(sx + 3, 0));
	m_z->setPosition(getPosition() + Neo::Vector2(sx*2 + 6, 0));
}

void Vector3Edit::update(float dt)
{
	m_x->update(dt);
	m_y->update(dt);
	m_z->update(dt);

	Neo2D::Gui::Widget::update(dt);
}

void Vector3Edit::draw(const Neo::Vector2& offset)
{
	m_x->draw(offset);
	m_y->draw(offset);
	m_z->draw(offset);
}

void Vector3Edit::setVector(const Neo::Vector3& v)
{
	char buf[64];
	snprintf(buf, sizeof(buf), "%f", v.x);
	m_x->setLabel(buf);
	
	snprintf(buf, sizeof(buf), "%f", v.y);
	m_y->setLabel(buf);
	
	snprintf(buf, sizeof(buf), "%f", v.z);
	m_z->setLabel(buf);

	m_x->setCaret(0);
	m_y->setCaret(0);
	m_z->setCaret(0);
}

Neo::Vector3 Vector3Edit::getVector() const
{
	Neo::Vector3 retval;
	sscanf(m_x->getLabel(), "%f", &retval.x);
	sscanf(m_y->getLabel(), "%f", &retval.y);
	sscanf(m_z->getLabel(), "%f", &retval.z);

	return retval;
}

// Vector4
Vector4Edit::Vector4Edit(int x,
						 int y,
						 unsigned int w,
						 unsigned int h,
						 const char* label,
						 const shared_ptr<Neo2D::Object2D>& parent,
						 const shared_ptr<Neo2D::Gui::Theme>& theme) :
	m_x(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
	m_y(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
	m_z(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
	m_w(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
	VectorEdit(x, y, w, h, label, parent)
{
	m_cb = [this](Neo2D::Gui::Widget&, void*) {
		doCallback();
	};
	
	m_x->setCallback(m_cb, nullptr);
	m_y->setCallback(m_cb, nullptr);
	m_z->setCallback(m_cb, nullptr);
	m_w->setCallback(m_cb, nullptr);
}

void Vector4Edit::updateContent()
{
	float sx = getSize().x / 4;
	m_x->setSize(Neo::Vector2(sx, getSize().y));
	m_y->setSize(Neo::Vector2(sx, getSize().y));
	m_z->setSize(Neo::Vector2(sx, getSize().y));
	m_w->setSize(Neo::Vector2(sx, getSize().y));
	
	m_x->setPosition(getPosition());

	// FIXME: Configurable spacing!
	m_y->setPosition(getPosition() + Neo::Vector2(sx + 3, 0));
	m_z->setPosition(getPosition() + Neo::Vector2(sx*2 + 6, 0));
	m_w->setPosition(getPosition() + Neo::Vector2(sx*3 + 9, 0));	
}

void Vector4Edit::update(float dt)
{
	m_x->update(dt);
	m_y->update(dt);
	m_z->update(dt);
	m_w->update(dt);

	Neo2D::Gui::Widget::update(dt);
}

void Vector4Edit::draw(const Neo::Vector2& offset)
{
	m_x->draw(offset);
	m_y->draw(offset);
	m_z->draw(offset);
	m_w->draw(offset);
}

void Vector4Edit::setVector(const Neo::Vector4& v)
{
	char buf[64];
	snprintf(buf, sizeof(buf), "%f", v.x);
	m_x->setLabel(buf);
	
	snprintf(buf, sizeof(buf), "%f", v.y);
	m_y->setLabel(buf);
	
	snprintf(buf, sizeof(buf), "%f", v.z);
	m_z->setLabel(buf);

	snprintf(buf, sizeof(buf), "%f", v.w);
	m_w->setLabel(buf);

	m_x->setCaret(0);
	m_y->setCaret(0);
	m_z->setCaret(0);
	m_w->setCaret(0);
}

Neo::Vector4 Vector4Edit::getVector() const
{
	Neo::Vector4 retval;
	sscanf(m_x->getLabel(), "%f", &retval.x);
	sscanf(m_y->getLabel(), "%f", &retval.y);
	sscanf(m_z->getLabel(), "%f", &retval.z);
	sscanf(m_w->getLabel(), "%f", &retval.w);

	return retval;
}
