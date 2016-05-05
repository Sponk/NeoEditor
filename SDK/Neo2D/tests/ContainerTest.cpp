#include <gtest/gtest.h>
#include <Container.h>

class TestFilter : public Neo2D::Gui::FilterStrategy
{
public:
	void updateVisibility(std::vector<shared_ptr<Neo2D::Gui::Widget>>& v)
	{
		for(auto w : v)
			w->setActive(false);
	}
};

TEST(ContainerTest, FilterTest)
{
	shared_ptr<Neo2D::Gui::Container> c = make_shared<Neo2D::Gui::Container>(0,0,0,0,nullptr);
	c->addWidget(make_shared<Neo2D::Gui::Widget>(0,0,5,5,"", c));
	c->addWidget(make_shared<Neo2D::Gui::Widget>(1,0,5,5,"", c));
	c->setFilter(make_shared<TestFilter>());

	c->updateFilter();

	ASSERT_FALSE(c->getWidget(0)->isActive());
	ASSERT_FALSE(c->getWidget(1)->isActive());
}

class TestLayout : public Neo2D::Gui::LayoutStrategy
{
public:
	void updateLayout(std::vector<shared_ptr<Neo2D::Gui::Widget>>& v)
	{
		for(auto w : v)
			w->setActive(false);
	}
};

TEST(ContainerTest, LayoutTest)
{
	shared_ptr<Neo2D::Gui::Container> c = make_shared<Neo2D::Gui::Container>(0,0,0,0,nullptr);
	c->addWidget(make_shared<Neo2D::Gui::Widget>(0,0,5,5,"",c));
	c->addWidget(make_shared<Neo2D::Gui::Widget>(1,0,5,5,"",c));

	c->setLayout(make_shared<TestLayout>());
	c->updateLayout();

	ASSERT_FALSE(c->getWidget(0)->isActive());
	ASSERT_FALSE(c->getWidget(1)->isActive());
}

TEST(ContainerTest, ParentingTest)
{
	auto c = make_shared<Neo2D::Gui::Container>(0,0,0,0,nullptr);
	auto widget = make_shared<Neo2D::Gui::Widget>(0,0,0,0,nullptr, nullptr);

	c->addWidget(widget);
	EXPECT_EQ(c.get(), widget->getParent().lock().get());
}

TEST(ContainerTest, ContentSizeTest)
{
	auto c = make_shared<Neo2D::Gui::Container>(0,0,0,0,nullptr);
	c->addWidget(make_shared<Neo2D::Gui::Widget>(-10,-10,20,20,nullptr,nullptr));
	c->addWidget(make_shared<Neo2D::Gui::Widget>(90,0,10,10,nullptr,nullptr));

	EXPECT_EQ(110, c->calculateContentSize().x);
	EXPECT_EQ(20, c->calculateContentSize().y);
}