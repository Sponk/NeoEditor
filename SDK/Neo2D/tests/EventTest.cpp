#include <gtest/gtest.h>
#include <Container.h>
#include <InputContext.h>
#include <MouseEvents.h>
#include <Canvas.h>

using namespace Neo;

class TestEvent : public Neo2D::Gui::Event
{
public:
	TestEvent(Neo2D::Gui::Widget& p, std::function<void(Neo2D::Gui::Widget&, const Event&, void*)> cb, void* d) : 
		Event(p, cb, d) {}
	virtual void update(float dt)
	{
		handle();
	}
};

TEST(EventTest, EventDetection)
{
	bool callbackCalled = false;
	Neo2D::Gui::Widget widget(0,0,0,0,nullptr);

	shared_ptr<Neo2D::Gui::Event> event = make_shared<TestEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { *((bool*)f) = true; }, &callbackCalled);
	widget.registerEvent(event);
	widget.update(0);

	ASSERT_TRUE(callbackCalled);
}

TEST(EventTest, ForwardEventDetection)
{
	int callbackCalled = 0;
	Neo2D::Gui::Container c(0,0,0,0,nullptr);
	shared_ptr<Neo2D::Gui::Widget> widget = make_shared<Neo2D::Gui::Widget>(0,0,0,0,nullptr);

	c.addWidget(widget);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<TestEvent>(*widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { *((bool*)f) = true; }, &callbackCalled);

	// Also checks how often the callback got called
	widget->registerEvent(event);
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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseOverEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);
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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseOverEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);
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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseLeaveEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);
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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseLeaveEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);

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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseLeftClickEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);

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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseMiddleClickEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);

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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseRightClickEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);

	mouse.keyDown(MOUSE_BUTTON_RIGHT); // Simulate click
	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.moveCursor(Vector2(16, 16)); // Place cursor outside widget
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);

	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}

TEST(EventTest, MouseLeftReleaseTest)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::NeoEngine::getInstance()->setInputContext(&input);
	Mouse& mouse = input.getMouse();

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseLeftReleaseEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);

	mouse.keyDown(MOUSE_BUTTON_LEFT); // Simulate click
	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.keyUp(MOUSE_BUTTON_LEFT); // Simulate release
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);

	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}

TEST(EventTest, MouseMiddleReleaseTest)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::NeoEngine::getInstance()->setInputContext(&input);
	Mouse& mouse = input.getMouse();

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseMiddleReleaseEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);

	mouse.keyDown(MOUSE_BUTTON_MIDDLE); // Simulate click
	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.keyUp(MOUSE_BUTTON_MIDDLE); // Simulate release
	widget.update(0);

	ASSERT_EQ(1, callbackCalled);

	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}

TEST(EventTest, MouseRightReleaseTest)
{
	int callbackCalled = 0;

	InputContextDummy input;
	Neo::NeoEngine::getInstance()->setInputContext(&input);
	Mouse& mouse = input.getMouse();

	Neo2D::Gui::Widget widget(0, 0, 15, 15, nullptr);
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseRightReleaseEvent>(widget, [](Neo2D::Gui::Widget& w, const Neo2D::Gui::Event&, void* f) { (*((int*)f))++; }, &callbackCalled);

	widget.registerEvent(event);

	mouse.keyDown(MOUSE_BUTTON_RIGHT); // Simulate click
	mouse.moveCursor(Vector2(5, 5)); // Place cursor inside widget
	widget.update(0);

	mouse.keyUp(MOUSE_BUTTON_RIGHT); // Simulate release
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
	shared_ptr<Neo2D::Gui::Event> event = make_shared<Neo2D::Gui::MouseMoveEvent>(widget, [](Neo2D::Gui::Widget &w, const Neo2D::Gui::Event &e, void *f)
	{ *((Neo::Vector2 *) f) = dynamic_cast<const Neo2D::Gui::MouseMoveEvent&>(e).getDelta(); }, &callbackDelta);

	widget.registerEvent(event);

	mouse.moveCursor(Vector2(0, 0)); // Place cursor inside widget
	widget.update(0);

	mouse.moveCursor(Neo::Vector2(16, 16)); // Move cursor
	widget.update(0);

	ASSERT_EQ(Neo::Vector2(16,16), callbackDelta);
	Neo::NeoEngine::getInstance()->setInputContext(nullptr);
}

TEST(EventTest, CanvasTest)
{
	static int called = 0;
	class TestWidget : public Neo2D::Gui::Widget
	{
	public:
		TestWidget(int x, int y, unsigned int w, unsigned int h, shared_ptr<Object2D> parent)
		: Widget(x, y, w, h, parent) {}

		void update(float dt)
		{
			called++;
		}
	};

	Neo2D::Canvas canvas;
	canvas.addObject2D(make_shared<TestWidget>(0,0,0,0, nullptr));

	canvas.update(0.0f);
	ASSERT_EQ(1, called);
}