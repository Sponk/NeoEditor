#ifndef __COLOR_EDIT_H
#define __COLOR_EDIT_H

#include <Button.h>
#include "NativeToolset.h"

class ColorEdit : public Neo2D::Gui::Widget
{
	std::function<void(Neo2D::Gui::Widget& w, void* d)> m_cb;
	Neo::Vector4 m_color;
	shared_ptr<Neo2D::Gui::Button> m_button;
	shared_ptr<NativeToolset> m_toolset;

public:
	ColorEdit(int x,
		  int y,
		  unsigned int w,
		  unsigned int h,
		  const char* label,
		  const shared_ptr<Neo2D::Object2D>& parent,
		  const shared_ptr<NativeToolset>& tools,
		  const shared_ptr<Neo2D::Gui::Theme>& theme = nullptr);
	
	virtual void update(float dt) override;
	virtual void draw(const Neo::Vector2& offset) override;
	void setColor(const Neo::Vector3& v) { m_color = v; }
	Neo::Vector4 getColor() const { return m_color; }
	
	void setPosition(const Neo::Vector2& pos)
	{
		m_button->setPosition(pos);
		Neo2D::Gui::Widget::setPosition(pos);
	}
	
	void setSize(const Neo::Vector2& sz)
	{
		m_button->setSize(sz);
		Neo2D::Gui::Widget::setSize(sz);
	}
};

#endif
