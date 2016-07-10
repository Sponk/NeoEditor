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
	void updateLayout(const Neo2D::Gui::Widget& wdg, std::vector<shared_ptr<Neo2D::Gui::Widget>>& v, const Neo::Vector2& offset)
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

TEST(ContainerTest, ContentSizeNegativePosTest)
{
	auto c = make_shared<Neo2D::Gui::Container>(0,0,0,0,nullptr);
	c->addWidget(make_shared<Neo2D::Gui::Widget>(-10,-10,20,20,nullptr,nullptr));
	c->addWidget(make_shared<Neo2D::Gui::Widget>(90,0,10,10,nullptr,nullptr));

	auto invisible = make_shared<Neo2D::Gui::Widget>(90,0,10,10,nullptr,nullptr);
	invisible->setInvisible(true);
	c->addWidget(invisible);

	EXPECT_EQ(100, c->calculateContentSize().x);
	EXPECT_EQ(10, c->calculateContentSize().y);
}

TEST(ContainerTest, ContentSizePosTest)
{
	auto c = make_shared<Neo2D::Gui::Container>(2,2,0,0,nullptr);
	c->addWidget(make_shared<Neo2D::Gui::Widget>(10,10,0,0,nullptr,nullptr));
	c->addWidget(make_shared<Neo2D::Gui::Widget>(20,50,0,0,nullptr,nullptr));

	auto invisible = make_shared<Neo2D::Gui::Widget>(90,0,10,10,nullptr,nullptr);
	invisible->setInvisible(true);
	c->addWidget(invisible);

	EXPECT_EQ(18, c->calculateContentSize().x);
	EXPECT_EQ(48, c->calculateContentSize().y);
}

TEST(ContainerTest, GetNonExistingWidget)
{
	auto c = make_shared<Neo2D::Gui::Container>(0,0,0,0, nullptr);
	EXPECT_EQ(nullptr, c->getWidget(123));
}

class DrawCounter : public Neo2D::Gui::Theme
{
public:
	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset) override
	{
		counter++;
	}

	int counter = 0;
};

TEST(ContainerTest, Draw)
{
	auto c = make_shared<Neo2D::Gui::Container>(0,0,0,0, nullptr);


	auto visibleTheme = make_shared<DrawCounter>();
	auto invisibleTheme = make_shared<DrawCounter>();

	auto visibleWidget = make_shared<Neo2D::Gui::Widget>(0,0,0,0,"", nullptr, visibleTheme);
	auto invisibleWidget = make_shared<Neo2D::Gui::Widget>(0,0,0,0,"", nullptr, invisibleTheme);

	visibleWidget->setInvisible(false);
	invisibleWidget->setInvisible(true);

	c->addWidget(visibleWidget);
	c->addWidget(invisibleWidget);

	// Draw
	c->draw(Neo::Vector2());

	// Actually test
	EXPECT_EQ(1, visibleTheme->counter);
	EXPECT_EQ(0, invisibleTheme->counter);
}

TEST(ContainerTest, DrawInvisibleContainer)
{
	auto c = make_shared<Neo2D::Gui::Container>(0,0,0,0, nullptr);

	auto visibleTheme = make_shared<DrawCounter>();
	auto visibleWidget = make_shared<Neo2D::Gui::Widget>(0,0,0,0,"", nullptr, visibleTheme);

	visibleWidget->setInvisible(false);

	c->addWidget(visibleWidget);
	c->setInvisible(true);

	// Draw
	c->draw(Neo::Vector2());

	// Actually test
	EXPECT_EQ(0, visibleTheme->counter);
}

// Handle test
class AlwaysHandleWidget : public Neo2D::Gui::Widget
{
public:
	AlwaysHandleWidget() : Neo2D::Gui::Widget(0,0,0,0,"", nullptr) {}
	bool handle(const Neo2D::Gui::Event& e) override
	{
		return true;
	}
};

class NeverHandleWidget : public Neo2D::Gui::Widget
{
public:
	NeverHandleWidget() : Neo2D::Gui::Widget(0,0,0,0,"", nullptr) {}
	bool handle(const Neo2D::Gui::Event& e) override
	{
		return false;
	}
};

TEST(ContainerTest, Handle)
{
	auto c = make_shared<Neo2D::Gui::Container>(0,0,0,0, nullptr);

	auto event = Neo2D::Gui::Event(c, nullptr, nullptr);
	c->addWidget(make_shared<AlwaysHandleWidget>());
	c->addWidget(make_shared<NeverHandleWidget>());

	EXPECT_TRUE(c->handle(event));
}