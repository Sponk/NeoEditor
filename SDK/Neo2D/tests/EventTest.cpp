#include <gtest/gtest.h>
#include <Container.h>
#include <InputContext.h>
#include <InputMapping.h>

using namespace Neo;

#define MOUSE_POSITION_DEFINED
static Neo::Vector2 g_mousePosition(15, 15);
Neo::Vector2 getMousePosition()
{
	return g_mousePosition;
}

#include <MouseEvents.h>

class TestEvent : public Neo2D::Gui::Event
{
public:
	virtual void update(Neo2D::Gui::Widget &w, float dt)
	{
		handle(w);
	}
};

TEST(EventTest, EventDetection)
{
	bool callbackCalled = false;
	Neo2D::Gui::Widget widget(0,0,0,0,nullptr);

	shared_ptr<TestEvent> event = make_shared<TestEvent>();
	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { *((bool*)f) = true; }, &callbackCalled);
	widget.update(0);

	ASSERT_TRUE(callbackCalled);
}

TEST(EventTest, ForwardEventDetection)
{
	int callbackCalled = 0;
	Neo2D::Gui::Container c(0,0,0,0,nullptr);
	shared_ptr<Neo2D::Gui::Widget> widget = make_shared<Neo2D::Gui::Widget>(0,0,0,0,nullptr);

	c.addWidget(widget);
	shared_ptr<TestEvent> event = make_shared<TestEvent>();

	// Also checks how often the callback got called
	widget->registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);
	c.update(0);

	ASSERT_EQ(1, callbackCalled);
}

TEST(EventTest, MouseOverTest_false)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::Mouse& mouse = input.getMouse();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	mouse.moveCursor(Vector2(15, 15));

	Neo2D::Gui::Widget widget(0, 0, 3, 3, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseOverEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);
	widget.update(0);
	widget.update(0);

	ASSERT_EQ(0, callbackCalled);
}


TEST(EventTest, MouseOverTest_true)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::Mouse& mouse = input.getMouse();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	mouse.moveCursor(Vector2(15, 15));

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseOverEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);
	widget.update(0);
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);
}

TEST(EventTest, MouseLeaveTest_false)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::Mouse& mouse = input.getMouse();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseLeaveEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);
	widget.update(0);
	widget.update(0);

	ASSERT_EQ(0, callbackCalled);
}

TEST(EventTest, MouseLeaveTest_true)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::Mouse& mouse = input.getMouse();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseLeaveEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.moveCursor(Vector2(16, 16)); // Place cursor outside widget
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);
}

TEST(EventTest, MouseLeftClickTest)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::NeoEngine::getInstance()->setInputContext(&input);
	Mouse& mouse = input.getMouse();

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseLeftClickEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	mouse.keyDown(MOUSE_BUTTON_LEFT); // Simulate click
	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.moveCursor(Vector2(16, 16)); // Place cursor outside widget
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);

	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}

TEST(EventTest, MouseMiddleClickTest)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::NeoEngine::getInstance()->setInputContext(&input);
	Mouse& mouse = input.getMouse();

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseMiddleClickEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	mouse.keyDown(MOUSE_BUTTON_MIDDLE); // Simulate click
	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.moveCursor(Vector2(16, 16)); // Place cursor outside widget
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);

	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}

TEST(EventTest, MouseRightClickTest)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::NeoEngine::getInstance()->setInputContext(&input);
	Neo::Mouse& mouse = input.getMouse();

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseRightClickEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	mouse.keyDown(MOUSE_BUTTON_RIGHT); // Simulate click
	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.moveCursor(Vector2(16, 16)); // Place cursor outside widget
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);

	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}

TEST(EventTest, MouseMoveTest)
{
	Neo::Vector2 callbackDelta;
	InputContextDummy input;
	Neo::Mouse& mouse = input.getMouse();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseMoveEvent>();

	widget.registerEvent(event, [](Neo2D::Gui::Widget &w, const Neo2D::Gui::Event &e, void *f)
	{ *((Neo::Vector2 *) f) = dynamic_cast<const Neo2D::Gui::MouseMoveEvent&>(e).getDelta(); }, &callbackDelta);

	mouse.moveCursor(Vector2(0, 0)); // Place cursor inside widget
	widget.update(0);

	mouse.moveCursor(Neo::Vector2(16, 16)); // Move cursor
	widget.update(0);

	ASSERT_EQ(Neo::Vector2(16,16), callbackDelta);
	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}