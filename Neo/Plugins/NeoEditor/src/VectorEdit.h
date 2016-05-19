#ifndef __VECTOR_EDIT_H
#define __VECTOR_EDIT_H

#include <EditField.h>

class Vector3Edit : public Neo2D::Gui::Widget
{
	shared_ptr<Neo2D::Gui::EditField> m_x, m_y, m_z;
public:
	Vector3Edit(int x,
			  int y,
			  unsigned int w,
			  unsigned int h,
			  const char* label,
			  const shared_ptr<Neo2D::Object2D>& parent,
			  const shared_ptr<Neo2D::Gui::Theme>& theme = nullptr) :
		m_x(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
		m_y(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
		m_z(make_shared<Neo2D::Gui::EditField>(0,0,0,0, nullptr, parent, theme)),
		Neo2D::Gui::Widget(x, y, w, h, label, parent)
	{}

	virtual void update(float dt) override;
	virtual void draw(const Neo::Vector2& offset) override;
	void setVector(const Neo::Vector3& v);
	Neo::Vector3 getVector() const;
};

#endif
