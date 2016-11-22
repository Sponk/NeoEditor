#ifndef __VECTOR_EDIT_H
#define __VECTOR_EDIT_H

#include <EditField.h>

class VectorEdit : public Neo2D::Gui::Widget
{
	virtual void updateContent() = 0;
public:
	
	VectorEdit(int x, int y, unsigned int w, unsigned int h, const char* label,
		   const shared_ptr<Neo2D::Object2D>& parent, const shared_ptr<Neo2D::Gui::Theme>& theme = nullptr) :
		   
		   Neo2D::Gui::Widget(x, y, w, h, label, parent, theme) {}
	
	void setPosition(const Neo::Vector2& position)
	{
		Neo2D::Gui::Widget::setPosition(position);
		updateContent();
	}
	
	void setSize(const Neo::Vector2& sz)
	{
		Neo2D::Gui::Widget::setSize(sz);
		updateContent();
	}
};

class Vector3Edit : public VectorEdit
{
	std::function<void(Neo2D::Gui::Widget& w, void* d)> m_cb;
	shared_ptr<Neo2D::Gui::EditField> m_x, m_y, m_z;

	void updateContent();
	
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

class Vector4Edit : public VectorEdit
{
	std::function<void(Neo2D::Gui::Widget& w, void* d)> m_cb;
	shared_ptr<Neo2D::Gui::EditField> m_x, m_y, m_z, m_w;

	void updateContent();
	
public:
	Vector4Edit(int x,
				int y,
				unsigned int w,
				unsigned int h,
				const char* label,
				const shared_ptr<Neo2D::Object2D>& parent,
				const shared_ptr<Neo2D::Gui::Theme>& theme = nullptr);
	
	virtual void update(float dt) override;
	virtual void draw(const Neo::Vector2& offset) override;
	void setVector(const Neo::Vector4& v);
	Neo::Vector4 getVector() const;
};

#endif
