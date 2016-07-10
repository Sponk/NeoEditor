#include <gtest/gtest.h>
#include <Scrollbar.h>
#include <CommonEvents.h>
#include "InputContextFixture.h"

using namespace Neo;
using namespace Neo2D;
using namespace Gui;


class ScrollbarTest: public InputContextFixture
{};

class TestScrollbar : public Scrollbar
{
public:

	using Scrollbar::getKnob;
	TestScrollbar(int x,
				  int y,
				  unsigned int w,
				  unsigned int h,
				  const shared_ptr<Object2D>& parent,
				  const shared_ptr<Theme>& knobtheme = nullptr,
				  const shared_ptr<Theme>& background = nullptr)
		: Scrollbar(x, y, w, h, parent, SCROLLBAR_HORIZONTAL, knobtheme, background)
	{ }
};

TEST_F(ScrollbarTest, ValueTest)
{
	auto sb = make_shared<Scrollbar>(0,0,100,100, nullptr, SCROLLBAR_HORIZONTAL, nullptr);

	sb->setRange(Neo::Vector2(0, 200));
	sb->setValue(101);
	EXPECT_EQ(100, sb->getValue());
}

TEST_F(ScrollbarTest, DragTestHorizontal)
{
	auto sb = make_shared<Scrollbar>(0,0,100,100, nullptr, SCROLLBAR_HORIZONTAL, nullptr);
	sb->setRange(Neo::Vector2(0, 200));

	MouseMoveEvent movement(sb, nullptr, nullptr);
	movement.setDelta(Neo::Vector2(100, 10));

	sb->handle(MouseLeftClickEvent(sb, nullptr, nullptr));
	sb->handle(movement);

	EXPECT_EQ(100, sb->getValue());
}

TEST_F(ScrollbarTest, DragTestVertical)
{
	auto sb = make_shared<Scrollbar>(0,0,100,100, nullptr, SCROLLBAR_VERTICAL, nullptr);
	sb->setRange(Neo::Vector2(0, 200));

	MouseMoveEvent movement(sb, nullptr, nullptr);
	movement.setDelta(Neo::Vector2(100, 10));

	sb->handle(MouseLeftClickEvent(sb, nullptr, nullptr));
	sb->handle(movement);

	EXPECT_EQ(20, sb->getValue());
}

TEST_F(ScrollbarTest, DISABLED_DeselectTest)
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

TEST_F(ScrollbarTest, KnobSizeSmallerTest)
{
	auto sb = make_shared<TestScrollbar>(0,0,100,100,nullptr,nullptr);
	sb->setRange(Neo::Vector2(0, 200));
	EXPECT_EQ(50, sb->calculateKnobsize());
}

TEST_F(ScrollbarTest, KnobSizeBiggerTest)
{
	auto sb = make_shared<TestScrollbar>(0,0,100,100,nullptr,nullptr);
	sb->setRange(Neo::Vector2(0, 50));
	EXPECT_EQ(100, sb->calculateKnobsize());
}

TEST_F(ScrollbarTest, MoveNotSelected)
{
	auto sb = make_shared<TestScrollbar>(0,0,100,100, nullptr, nullptr);
	m_mouse.moveCursor(Vector2(10,10));
	m_mouse.flushDirection();
	sb->update(0);

	EXPECT_EQ(0, sb->getValue());
}

TEST_F(ScrollbarTest, SelectMoveDeselect)
{
	auto sb = make_shared<TestScrollbar>(0,0,100,100, nullptr, nullptr);
	sb->setRange(Neo::Vector2(0, 200));
	sb->setDirection(SCROLLBAR_VERTICAL);

	m_mouse.keyDown(MOUSE_BUTTON_LEFT);
	sb->update(0);
	EXPECT_EQ(WIDGET_SELECTED, sb->getState());

	m_mouse.moveCursor(Vector2(5,5));
	sb->update(0);
	EXPECT_EQ(10, sb->getValue());

	m_mouse.keyUp(MOUSE_BUTTON_LEFT);
	sb->update(0);
	EXPECT_EQ(WIDGET_NORMAL, sb->getState());
}