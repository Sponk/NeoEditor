#include "VectorEdit.h"

void Vector3Edit::update(float dt)
{
	float sx = getSize().x / 3;
	m_x->setSize(Neo::Vector2(sx, getSize().y));
	m_y->setSize(Neo::Vector2(sx, getSize().y));
	m_z->setSize(Neo::Vector2(sx, getSize().y));
	
	m_x->setPosition(getPosition());

	// FIXME: Configurable spacing!
	m_y->setPosition(getPosition() + Neo::Vector2(sx + 3, 0));
	m_z->setPosition(getPosition() + Neo::Vector2(sx*2 + 6, 0));

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
}

Neo::Vector3 Vector3Edit::getVector() const
{
	Neo::Vector3 retval;
	sscanf(m_x->getLabel(), "%f", &retval.x);
	sscanf(m_y->getLabel(), "%f", &retval.y);
	sscanf(m_z->getLabel(), "%f", &retval.z);

	return retval;
}
