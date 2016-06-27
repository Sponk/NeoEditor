#ifndef NEO_CONSOLE_H
#define NEO_CONSOLE_H

#include <Widget.h>
#include <EditField.h>
#include <Label.h>
#include <ScrollPanel.h>
#include <vector>

class Console : public Neo2D::Gui::Widget
{
	shared_ptr<Neo2D::Gui::EditField> commandEdit;
	shared_ptr<Neo2D::Gui::Label> commandOutput;
	shared_ptr<Neo2D::Gui::ScrollPanel> scrollPanel;

public:
	Console(int x,
			int y,
			unsigned int w,
			unsigned int h,
			const shared_ptr<Neo2D::Object2D>& parent);

	virtual void init() override;
	virtual bool handle(const Neo2D::Gui::Event& e) override;
	virtual void draw(const Neo::Vector2& offset) override;
	virtual void update(float dt) override;

	void write(const char* str);
};

#endif //NEO_SCENEVIEW_H
