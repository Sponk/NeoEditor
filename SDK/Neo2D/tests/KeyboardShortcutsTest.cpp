#include <gtest/gtest.h>
#include <KeyboardShortcuts.h>
#include <CommonEvents.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

class KeyboardShortcutsTest : public ::testing::Test
{
public:
	InputContextDummy m_input;
	Mouse& m_mouse = m_input.getMouse();
	Keyboard& m_kbd = m_input.getKeyboard();
protected:
	virtual void SetUp()
	{
		NeoEngine::getInstance()->setInputContext(&m_input);
	}

	virtual void TearDown()
	{
		NeoEngine::getInstance()->setInputContext(nullptr);
	}
};
TEST_F(KeyboardShortcutsTest, TestKey)
{
	int called = 0;
	Shortcut sc({KEY_LCONTROL, KEY_A}, [&called](void*){ called++; }, nullptr);
	auto kbs = make_shared<KeyboardShortcuts>();

	kbs->addShortcut(sc);

	auto keyControl = KeyPressEvent(kbs, nullptr, nullptr);
	auto keyA = KeyPressEvent(kbs, nullptr, nullptr);

	keyControl.setKey(KEY_LCONTROL);
	keyA.setKey(KEY_A);

	kbs->handle(keyControl);
	kbs->handle(keyA);

	EXPECT_EQ(1, called);
}

TEST_F(KeyboardShortcutsTest, TestKeyWrong)
{
	int called = 0;
	Shortcut sc({KEY_LCONTROL, KEY_A}, [&called](void*){ called++; }, nullptr);
	auto kbs = make_shared<KeyboardShortcuts>();

	kbs->addShortcut(sc);

	auto keyControl = KeyPressEvent(kbs, nullptr, nullptr);
	auto keyB = KeyPressEvent(kbs, nullptr, nullptr);

	keyControl.setKey(KEY_LCONTROL);
	keyB.setKey(KEY_B);

	kbs->handle(keyControl);
	kbs->handle(keyB);

	EXPECT_EQ(0, called);
}


TEST_F(KeyboardShortcutsTest, InputKey)
{
	int called = 0;
	Shortcut sc({KEY_LCONTROL, KEY_A}, [&called](void*){ called++; }, nullptr);
	auto kbs = make_shared<KeyboardShortcuts>();
	kbs->addShortcut(sc);

	m_kbd.keyDown(KEY_LCONTROL);
	m_kbd.keyDown(KEY_A);

	kbs->update(0);
	EXPECT_EQ(1, called);

	m_kbd.keyUp(KEY_LCONTROL);
	m_kbd.keyUp(KEY_A);

	kbs->update(0);
	EXPECT_EQ(1, called);

	m_input.flush();
	kbs->update(0);
	EXPECT_EQ(1, called);
}