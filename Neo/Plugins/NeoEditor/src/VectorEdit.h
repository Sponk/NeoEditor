#ifndef __VECTOR_EDIT_H
#define __VECTOR_EDIT_H

#include <EditField.h>

class Vector3Edit : public Neo2D::Gui::Widget
{
	std::function<void(Neo2D::Gui::Widget& w, void* d)> m_cb;
	shared_ptr<Neo2D::Gui::EditField> m_x, m_y, m_z;
public:
	Vector3Edit(int x,
				int y,
				unsigned int w,
				unsigned int h,
				const char* label,
				const shared_ptr<Neo2D::Object2D>& parent,
				const shared_ptr<Neo2D::Gui::Theme>& theme = nullptr);
	
	virtual void update(float dt) override;
	virtual void draw(const Neo::Vector2& offset) override;
	void setVector(const Neo::Vector3& v);
	Neo::Vector3 getVector() const;
};

#endif
