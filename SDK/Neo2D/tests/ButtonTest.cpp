#include <gtest/gtest.h>
#include <Button.h>
#include <ImageButton.h>
#include <NeoCore.h>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

TEST(ButtonTest, HoverTest)
{
	InputContextDummy input;
	Mouse& mouse = input.getMouse();
	NeoEngine::getInstance()->setInputContext(&input);

	auto btn = make_shared<Button>(15, 15, 10, 10, nullptr, nullptr);

	mouse.moveCursor(Vector2(15, 15));
	btn->update(0.0f);
	ASSERT_EQ(WIDGET_HOVER, btn->getState());

	mouse.moveCursor(Vector2(30, 30));
	btn->update(0.0f);
	ASSERT_EQ(WIDGET_NORMAL, btn->getState());
}

TEST(ButtonTest, CallbackTest)
{
	InputContextDummy input;
	Mouse& mouse = input.getMouse();
	NeoEngine::getInstance()->setInputContext(&input);
	int callbackCounter = 0;

	auto btn = make_shared<Button>(15, 15, 10, 10, nullptr, nullptr);
	btn->setCallback([](Widget& w, void* data) { (*((int*) data))++; }, &callbackCounter);

	mouse.moveCursor(Vector2(15, 15));
	mouse.keyDown(MOUSE_BUTTON_LEFT);
	btn->update(0.0f);

	mouse.keyUp(MOUSE_BUTTON_LEFT);
	btn->update(0.0);

	ASSERT_EQ(1, callbackCounter);
}

TEST(ImageButtonTest, Padding)
{
	ImageButton btn(0,0,10,10,"", nullptr);
	EXPECT_EQ(Vector2(0.5,0.5), btn.getPadding());
}