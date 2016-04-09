#include <gtest/gtest.h>
#include <CheckButton.h>
#include <NeoCore.h>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;

TEST(CheckButtonTest, ValueTest)
{
	InputContextDummy input;
	Mouse& mouse = input.getMouse();
	NeoEngine::getInstance()->setInputContext(&input);

	CheckButton btn(15, 15, 10, 10, nullptr, nullptr);

	mouse.moveCursor(Vector2(15, 15));
	mouse.keyDown(MOUSE_BUTTON_LEFT);
	btn.update(0.0f);

	mouse.keyUp(MOUSE_BUTTON_LEFT);
	btn.update(0.0);
	ASSERT_EQ(true, btn.getValue());

	mouse.keyDown(MOUSE_BUTTON_LEFT);
	btn.update(0.0f);

	mouse.keyUp(MOUSE_BUTTON_LEFT);
	btn.update(0.0);
	ASSERT_EQ(false, btn.getValue());
}