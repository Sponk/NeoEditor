#include <gtest/gtest.h>
#include <Menu.h>
#include <CommonEvents.h>

using namespace Neo2D;
using namespace Gui;

class TestSubmenu : public Submenu
{
public:
	TestSubmenu(const char* label, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme)
		: Submenu(label, parent, theme)
	{ }

	using Submenu::findChild;
	using Submenu::findSubmenu;
};


TEST(SubmenuTest, AddItemToplevelTest)
{
	auto menu = make_shared<Submenu>("New", nullptr);
	menu->setActive(true);

	auto child = menu->addItem("toplevel", nullptr);
	EXPECT_EQ(child, menu->findChild("toplevel"));
}

TEST(SubmenuTest, AddItemDeepTest)
{
	auto menu = make_shared<Submenu>("New", nullptr);
	menu->setActive(true);

	auto child = menu->addItem("/toplevel/second/third", nullptr);
	ASSERT_NE(nullptr, menu->findSubmenu("toplevel").get());
	ASSERT_NE(nullptr, menu->findSubmenu("toplevel")->findSubmenu("second").get());

	EXPECT_EQ(child, menu->findSubmenu("toplevel")->findSubmenu("second")->findChild("third"));
	EXPECT_EQ(menu->findSubmenu("toplevel"), menu->findSubmenu("toplevel")->findSubmenu("second")->getParent().lock());
	EXPECT_EQ(child->getParent().lock(), menu->findSubmenu("toplevel")->findSubmenu("second"));
}

TEST(SubmenuTest, AddMultiItemDeepTest)
{
	auto menu = make_shared<Submenu>("New", nullptr);
	menu->setActive(true);

	auto third = menu->addItem("/toplevel/second/third", nullptr);
	auto fourth = menu->addItem("/toplevel/second/fourth", nullptr);

	ASSERT_NE(nullptr, menu->findSubmenu("toplevel").get());
	ASSERT_NE(nullptr, menu->findSubmenu("toplevel")->findSubmenu("second").get());


	ASSERT_NE(nullptr, third->getParent().lock().get());
	ASSERT_NE(nullptr, fourth->getParent().lock().get());

	ASSERT_EQ(third, menu->findSubmenu("toplevel")->findSubmenu("second")->findChild("third"));
	ASSERT_EQ(fourth, menu->findSubmenu("toplevel")->findSubmenu("second")->findChild("fourth"));
}

TEST(SubmenuTest, TriggerTest)
{
	auto menu = make_shared<Submenu>("New", nullptr);
	menu->setActive(true);

	auto third = menu->addItem("/toplevel/second/third", nullptr);
	auto fourth = menu->addItem("/toplevel/second/fourth", nullptr);

	third->setActive(true);
	fourth->setActive(true);

	int callbackCounter = 0;
	fourth->setCallback([](Widget& w, void* data) { (*((int*) data))++; }, &callbackCounter);

	fourth->handle(MouseLeftReleaseEvent(fourth, nullptr, nullptr));
	EXPECT_EQ(1, callbackCounter);

	EXPECT_FALSE(third->isActive());
	EXPECT_FALSE(fourth->isActive());
}

TEST(SubmenuTest, DISABLED_SimpleHoverTest)
{
	auto menu = make_shared<Submenu>("New", nullptr);
	menu->setActive(true);

	menu->handle(MouseOverEvent(menu, nullptr, nullptr));
	EXPECT_TRUE(menu->isVisible());

	menu->handle(MouseDeselectEvent(menu, nullptr, nullptr));
	EXPECT_FALSE(menu->isVisible());
}

TEST(SubmenuTest, DISABLED_DeepHoverTest)
{
	auto menu = make_shared<Submenu>("New", nullptr);
	menu->setActive(true);

	// Add some items
	auto p1c1 = menu->addItem("/parent1/child1", nullptr);
	auto p1c2 = menu->addItem("/parent1/child2", nullptr);
	auto p2c1 = menu->addItem("/parent2/child1", nullptr);

	auto p1 = menu->findSubmenu("parent1");
	auto p2 = menu->findSubmenu("parent2");

	ASSERT_NE(nullptr, p1->getParent().lock().get());
	ASSERT_NE(nullptr, p2->getParent().lock().get());

	menu->handle(MouseOverEvent(menu, nullptr, nullptr));
	EXPECT_TRUE(menu->isVisible());
	EXPECT_FALSE(p1->isVisible());
	EXPECT_FALSE(p2->isVisible());
	EXPECT_EQ(WIDGET_HOVER, menu->getState());

	// Select p1
	p1->handle(MouseOverEvent(p1, nullptr, nullptr));
	EXPECT_FALSE(p1c1->isVisible());
	EXPECT_FALSE(p1c2->isVisible());
	EXPECT_FALSE(p2c1->isVisible());
	EXPECT_EQ(WIDGET_HOVER, p1->getState());

	// Select p2
	p2->handle(MouseOverEvent(p2, nullptr, nullptr));
	EXPECT_FALSE(p1c1->isVisible());
	EXPECT_FALSE(p1c2->isVisible());
	EXPECT_FALSE(p2c1->isVisible());

	menu->handle(MouseDeselectEvent(menu, nullptr, nullptr));
	EXPECT_FALSE(menu->isVisible());
	EXPECT_NE(WIDGET_SELECTED, menu->getState());
}

TEST(MenubarTest, TriggerTest)
{
	Neo::InputContextDummy input;
	Neo::Mouse& mouse = input.getMouse();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	int counter = 0;
	auto menubar = make_shared<Menubar>(0,0,100,10,nullptr);
	auto filemenu = make_shared<Submenu>("LALA", menubar);
	menubar->addMenu(filemenu);

	auto testmenu = filemenu->addItem("Test", [&counter](Widget& w, void* d) { counter++; });

	EXPECT_FALSE(filemenu->isActive());

	mouse.moveCursor(Neo::Vector2(10,5));
	menubar->update(0);

	mouse.keyDown(Neo::MOUSE_BUTTON_LEFT);
	menubar->update(0);
	mouse.flush();

	mouse.keyUp(Neo::MOUSE_BUTTON_LEFT);
	menubar->update(0);

	EXPECT_TRUE(filemenu->isActive());
	EXPECT_FALSE(testmenu->isActive());

	mouse.moveCursor(Neo::Vector2(10,20));
	menubar->update(0);
	menubar->update(0);

	EXPECT_TRUE(testmenu->isActive());
	EXPECT_EQ(WIDGET_HOVER, testmenu->getState());

	mouse.keyDown(Neo::MOUSE_BUTTON_LEFT);
	menubar->update(0);

	mouse.keyUp(Neo::MOUSE_BUTTON_LEFT);
	mouse.flush();

	menubar->update(0);

	EXPECT_EQ(1, counter);
	EXPECT_FALSE(filemenu->isActive());
	EXPECT_FALSE(testmenu->isActive());
}