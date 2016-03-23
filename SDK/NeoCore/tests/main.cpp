#include <gtest/gtest.h>
#include <NeoCore.h>

using namespace Neo;

class InputContextDummyProxy : public InputContextDummy
{
public:
	Keyboard& getKeyboard() { return InputContextDummy::getKeyboard(); }
	Mouse& getMouse() { return InputContextDummy::getMouse(); }
};

TEST(NeoCoreTest, InputDeviceTest)
{
	// Use keyboard since it is basically just an empty input device
	// which allocates some keys at the beginning
	Keyboard kbd;
	kbd.keyDown(KEY_DUMMY);

	ASSERT_TRUE(kbd.isKeyDown(KEY_DUMMY));
	ASSERT_TRUE(kbd.onKeyDown(KEY_DUMMY));
	ASSERT_FALSE(kbd.onKeyUp(KEY_DUMMY));

	// Clear the onKeyDown/up flag
	kbd.flush();

	// Test so onKeyDown does not fire more than once for every keypress
	kbd.keyDown(KEY_DUMMY);
	ASSERT_TRUE(kbd.isKeyDown(KEY_DUMMY));
	ASSERT_FALSE(kbd.onKeyDown(KEY_DUMMY));
	ASSERT_FALSE(kbd.onKeyUp(KEY_DUMMY));

	// Test onKeyUp event
	kbd.keyUp(KEY_DUMMY);

	ASSERT_FALSE(kbd.isKeyDown(KEY_DUMMY));
	ASSERT_FALSE(kbd.onKeyDown(KEY_DUMMY));
	ASSERT_TRUE(kbd.onKeyUp(KEY_DUMMY));
}

TEST(NeoCoreTest, MouseDeviceTest)
{
	Mouse mouse;
	mouse.moveCursor(Vector2(5.0, 5.0));

	ASSERT_EQ(mouse.getDirection(), Vector2(5.0, 5.0));
}

TEST(NeoCoreTest, InputMappingTest_keyboard)
{
	InputContextDummyProxy input;
	InputMapping map(input);
	Keyboard& kbd = input.getKeyboard();

	map.mapKey("Forward", KEY_W);

	ASSERT_FALSE(map.isKeyDown("Forward"));
	ASSERT_FALSE(map.onKeyDown("Forward"));
	ASSERT_FALSE(map.onKeyUp("Forward"));

	kbd.keyDown(KEY_W);
	ASSERT_TRUE(map.isKeyDown("Forward"));
	ASSERT_TRUE(map.onKeyDown("Forward"));
	ASSERT_FALSE(map.onKeyUp("Forward"));

}
int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}