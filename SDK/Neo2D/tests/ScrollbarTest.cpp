#include <gtest/gtest.h>
#include <Scrollbar.h>
#include <CommonEvents.h>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

class ScrollbarTest : public Scrollbar
{
public:

	using Scrollbar::getKnob;
	ScrollbarTest(int x,
				  int y,
				  unsigned int w,
				  unsigned int h,
				  const shared_ptr<Object2D>& parent,
				  const shared_ptr<Theme>& knobtheme = nullptr,
				  const shared_ptr<Theme>& background = nullptr)
		: Scrollbar(x, y, w, h, parent, SCROLLBAR_HORIZONTAL, knobtheme, background)
	{ }
};

TEST(ScrollbarTest, ValueTest)
{
	auto sb = make_shared<Scrollbar>(0,0,100,100, nullptr, SCROLLBAR_HORIZONTAL, nullptr);

	sb->setRange(Neo::Vector2(0, 200));
	sb->setValue(101);
	EXPECT_EQ(100, sb->getValue());
}

TEST(ScrollbarTest, DragTestHorizontal)
{
	auto sb = make_shared<Scrollbar>(0,0,100,100, nullptr, SCROLLBAR_HORIZONTAL, nullptr);
	sb->setRange(Neo::Vector2(0, 200));

	MouseMoveEvent movement(sb, nullptr, nullptr);
	movement.setDelta(Neo::Vector2(100, 10));

	sb->handle(MouseLeftClickEvent(sb, nullptr, nullptr));
	sb->handle(movement);

	EXPECT_EQ(100, sb->getValue());
}

TEST(ScrollbarTest, DragTestVertical)
{
	auto sb = make_shared<Scrollbar>(0,0,100,100, nullptr, SCROLLBAR_VERTICAL, nullptr);
	sb->setRange(Neo::Vector2(0, 200));

	MouseMoveEvent movement(sb, nullptr, nullptr);
	movement.setDelta(Neo::Vector2(100, 10));

	sb->handle(MouseLeftClickEvent(sb, nullptr, nullptr));
	sb->handle(movement);

	EXPECT_EQ(20, sb->getValue());
}

TEST(ScrollbarTest, DISABLED_DeselectTest)
{
	auto sb = make_shared<Scrollbar>(0,0,100,100, nullptr, SCROLLBAR_VERTICAL, nullptr);
	sb->setRange(Neo::Vector2(0, 200));
	sb->setDirection(SCROLLBAR_VERTICAL);

	MouseMoveEvent movement(sb, nullptr, nullptr);
	movement.setDelta(Neo::Vector2(100, 10));

	sb->handle(MouseLeftClickEvent(sb, nullptr, nullptr));
	sb->handle(movement);
	sb->handle(MouseLeaveEvent(sb, nullptr, nullptr));

	EXPECT_EQ(WIDGET_NORMAL, sb->getState());
}

TEST(ScrollbarTest, KnobSizeSmallerTest)
{
	auto sb = make_shared<ScrollbarTest>(0,0,100,100,nullptr,nullptr);
	sb->setRange(Neo::Vector2(0, 200));
	EXPECT_EQ(50, sb->calculateKnobsize());
}

TEST(ScrollbarTest, KnobSizeBiggerTest)
{
	auto sb = make_shared<ScrollbarTest>(0,0,100,100,nullptr,nullptr);
	sb->setRange(Neo::Vector2(0, 50));
	EXPECT_EQ(100, sb->calculateKnobsize());
}
