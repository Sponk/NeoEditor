#include <gtest/gtest.h>
#include <RadioButton.h>
#include <MouseEvents.h>

class RadioButtonTest : public Neo2D::Gui::RadioButton
{
public:
	RadioButtonTest(int x,
					int y,
					unsigned int w,
					unsigned int h,
					const char* label,
					const shared_ptr<Neo2D::Object2D>& parent)
		: RadioButton(x, y, w, h, label, parent)
	{}

	using Neo2D::Gui::RadioButton::getButtonWidget;
};

TEST(RadioButtonTest, RadioGroupTest)
{
	auto group = make_shared<Neo2D::Gui::RadioGroup>(0,0,0,0, nullptr);
	auto btn0 = make_shared<RadioButtonTest>(0,0,0,0, nullptr, nullptr);
	auto btn1 = make_shared<RadioButtonTest>(0,0,0,0, nullptr, nullptr);

	auto buttonWidget0 = btn0->getButtonWidget();
	auto buttonWidget1 = btn1->getButtonWidget();

	group->addWidget(btn0);
	group->addWidget(btn1);

	// Send a click to the button
	buttonWidget0.handle(Neo2D::Gui::MouseLeftReleaseEvent(*btn0.get(), nullptr, nullptr));
	EXPECT_TRUE(btn0->getValue());
	EXPECT_FALSE(btn1->getValue());

	// Send another click to the other button
	buttonWidget1.handle(Neo2D::Gui::MouseLeftReleaseEvent(*btn1.get(), nullptr, nullptr));
	EXPECT_TRUE(btn1->getValue());
	EXPECT_FALSE(btn0->getValue());
}

TEST(RadioButtonTest, ReselectTest)
{
	RadioButtonTest btn(0,0,0,0,nullptr,nullptr);
	auto buttonWidget = btn.getButtonWidget();

	buttonWidget.handle(Neo2D::Gui::MouseLeftReleaseEvent(btn, nullptr, nullptr));
	buttonWidget.handle(Neo2D::Gui::MouseLeftReleaseEvent(btn, nullptr, nullptr));

	EXPECT_TRUE(btn.getValue());
}