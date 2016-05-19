#include <gtest/gtest.h>
#include <VerticalLayout.h>
#include <Container.h>

using namespace Neo2D;
using namespace Gui;

TEST(VerticalLayoutTest, TestPositioning)
{	
	auto container = make_shared<Container>(10,10,100,200,nullptr);
	auto layout = make_shared<VerticalLayout>();

	auto w1 = make_shared<Widget>(0,0,0,0,nullptr,nullptr);
	auto w2 = make_shared<Widget>(0,0,0,0,nullptr,nullptr);

	container->addWidget(w1);
	container->addWidget(w2);
	container->setLayout(layout);
	
	container->updateLayout();
	
	EXPECT_EQ(10, w1->getPosition().x);
	EXPECT_EQ(10, w1->getPosition().y);

	EXPECT_EQ(10, w2->getPosition().x);
	EXPECT_EQ(110, w2->getPosition().y);
}

TEST(VerticalLayoutTest, TestScaling)
{
	auto container = make_shared<Container>(10,10,100,200,nullptr);
	auto layout = make_shared<VerticalLayout>();

	auto w1 = make_shared<Widget>(0,0,0,0,nullptr,nullptr);
	auto w2 = make_shared<Widget>(0,0,0,0,nullptr,nullptr);

	container->addWidget(w1);
	container->addWidget(w2);
	container->setLayout(layout);
	
	container->updateLayout();

	EXPECT_EQ(100, w1->getSize().x);
	EXPECT_EQ(100, w1->getSize().y);

	EXPECT_EQ(100, w2->getSize().x);
	EXPECT_EQ(100, w2->getSize().y);
}
