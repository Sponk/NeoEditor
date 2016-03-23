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
	c->addWidget(make_shared<Neo2D::Gui::Widget>(0,0,5,5,c));
	c->addWidget(make_shared<Neo2D::Gui::Widget>(1,0,5,5,c));
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
	c->addWidget(make_shared<Neo2D::Gui::Widget>(0,0,5,5,c));
	c->addWidget(make_shared<Neo2D::Gui::Widget>(1,0,5,5,c));

	c->setLayout(make_shared<TestLayout>());
	c->updateLayout();

	ASSERT_FALSE(c->getWidget(0)->isActive());
	ASSERT_FALSE(c->getWidget(1)->isActive());
}
