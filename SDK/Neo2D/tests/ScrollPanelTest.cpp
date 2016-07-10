#include <gtest/gtest.h>
#include <ScrollPanel.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

class TestPanel : public ScrollPanel
{
public:
	TestPanel() : ScrollPanel(0,0,100,100, nullptr) {}
	using ScrollPanel::updateScrollbarPlacement;
	using ScrollPanel::verticalScroll;
	using ScrollPanel::horizontalScroll;
};

TEST(ScrollPanelTest, ScrollbarPlacement)
{
	auto sp = make_shared<TestPanel>();
	sp->updateScrollbarPlacement();

	EXPECT_EQ(Vector2(0, sp->getSize().y), sp->horizontalScroll.getPosition());
	EXPECT_EQ(Vector2(sp->getSize().x, 0), sp->verticalScroll.getPosition());
}

TEST(ScrollPanelTest, ScrollbarOffset)
{
	// Set input context so event handling doesn't crash
	InputContextDummy input;
	Mouse& mouse = input.getMouse();
	NeoEngine::getInstance()->setInputContext(&input);

	auto sp = make_shared<TestPanel>();
	auto w1 = make_shared<Widget>(100,100,10,10, "", sp);
	auto w2 = make_shared<Widget>(0,0,10,10, "", sp);

	sp->addWidget(w1);
	sp->addWidget(w2);

	sp->update(0);

	sp->horizontalScroll.setValue(10);
	sp->verticalScroll.setValue(10);

	sp->update(0);

	EXPECT_EQ(Vector2(90,90), w1->getPosition());
	EXPECT_EQ(Vector2(-10,-10), w2->getPosition());
}