#include <gtest/gtest.h>
#include <KeyboardShortcuts.h>
#include <CommonEvents.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

TEST(KeyboardShortcutsTest, TestKey)
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

TEST(KeyboardShortcutsTest, TestKeyWrong)
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