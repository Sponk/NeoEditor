#include <gtest/gtest.h>
#include <CheckButton.h>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

TEST(CheckButtonTest, ValueTest)
{
	InputContextDummy input;
	Mouse& mouse = input.getMouse();
	NeoEngine::getInstance()->setInputContext(&input);

	auto btn = make_shared<CheckButton>(15, 15, 10, 10, nullptr, nullptr);

	mouse.moveCursor(Vector2(15, 15));
	btn->update(0.0f);

	mouse.keyDown(MOUSE_BUTTON_LEFT);
	btn->update(0.0f);

	mouse.keyUp(MOUSE_BUTTON_LEFT);
	btn->update(0.0);
	ASSERT_TRUE(btn->getValue());

	mouse.keyDown(MOUSE_BUTTON_LEFT);
	btn->update(0.0f);

	mouse.keyUp(MOUSE_BUTTON_LEFT);
	btn->update(0.0);
	ASSERT_FALSE(btn->getValue());
}