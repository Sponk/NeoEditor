#include <gtest/gtest.h>
#include <TreeView.h>

using namespace Neo2D;
using namespace Gui;

TEST(TreeViewTest, FindTest)
{
	auto view = make_shared<TreeView>(0,0,0,0,nullptr);

	view->getRoot()->addChild("some")->addChild("child")->addChild("in")->addChild("here");

	EXPECT_NE(nullptr, view->findNode("some"));
	EXPECT_NE(nullptr, view->findNode("child"));
	EXPECT_NE(nullptr, view->findNode("in"));
	EXPECT_NE(nullptr, view->findNode("here"));
	EXPECT_EQ(nullptr, view->findNode("doesntexist"));
}

TEST(TreeViewTest, SetSelectedTest)
{
	auto view = make_shared<TreeView>(0,0,0,0,nullptr);
	auto selected = view->getRoot()->addChild("some")->addChild("child")->addChild("in")->addChild("here");

	EXPECT_EQ(nullptr, view->getSelected());

	view->setSelected(selected->getLabel());

	EXPECT_TRUE(view->findNode("some")->isOpen());
	EXPECT_TRUE(view->findNode("child")->isOpen());
	EXPECT_TRUE(view->findNode("in")->isOpen());
}

TEST(TreeViewTest, ClearTest)
{
	auto view = make_shared<TreeView>(0,0,0,0,nullptr);
	auto selected = view->getRoot()->addChild("some")->addChild("child")->addChild("in")->addChild("here");

	view->getRoot()->clear();
	view->update(0);
}

TEST(TreeViewTest, SelectDeselect)
{
	Neo::RendererDummy renderer;
	Neo::InputContextDummy input;
	Neo::Mouse& mouse = input.getMouse();
	Neo::NeoEngine::getInstance()->setInputContext(&input);
	Neo::NeoEngine::getInstance()->setRenderer(&renderer);

	auto view = make_shared<TreeView>(0,0,100,100,nullptr);
	auto selected = view->getRoot()->addChild("some");

	// Make sure everything is open
	view->getRoot()->setOpen(true);

	// Update node placement
	view->draw(Neo::Vector2());

	int counter = 0;
	view->setCallback([&counter](Widget&, void*) { counter++; }, nullptr);

	mouse.moveCursor(Neo::Vector2(1,1));
	mouse.keyDown(Neo::MOUSE_BUTTON_LEFT);

	selected->update(0);
	view->update(0);

	EXPECT_EQ(1, counter);
}