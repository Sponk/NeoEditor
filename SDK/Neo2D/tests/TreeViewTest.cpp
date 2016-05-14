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