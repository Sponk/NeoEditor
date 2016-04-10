#include <gtest/gtest.h>
#include <EditField.h>
#include <CommonEvents.h>
#include <InputContext.h>

TEST(EditFieldTest, SelectFieldTest)
{
	Neo2D::Gui::EditField edit(0,0,0,0,nullptr,nullptr,nullptr);
	edit.handle(Neo2D::Gui::MouseLeftClickEvent(edit, nullptr, nullptr));

	EXPECT_EQ(Neo2D::Gui::WIDGET_SELECTED, edit.getState());
}

TEST(EditFieldTest, HoverFieldTest)
{
	Neo2D::Gui::EditField edit(0,0,0,0,nullptr,nullptr,nullptr);
	edit.handle(Neo2D::Gui::MouseOverEvent(edit, nullptr, nullptr));

	EXPECT_EQ(Neo2D::Gui::WIDGET_HOVER, edit.getState());
}

TEST(EditFieldTest, EnterCharacterTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,nullptr,nullptr,nullptr);
	Neo2D::Gui::CharacterInputEvent event(edit, nullptr, nullptr);

	kbd.setCharacter('K');
	event.update(0.0f);

	kbd.setCharacter('L');
	event.update(0.0f);

	EXPECT_EQ(2, edit.getCaret());
	EXPECT_STREQ("KL", edit.getLabel());
}

TEST(EditFieldTest, EnterCharacterMiddleTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"HELLO",nullptr,nullptr);
	Neo2D::Gui::CharacterInputEvent event(edit, nullptr, nullptr);

	edit.setCaret(2);

	kbd.setCharacter('K');
	event.update(0.0f);

	kbd.setCharacter('L');
	event.update(0.0f);

	EXPECT_EQ(4, edit.getCaret());
	EXPECT_STREQ("HEKLLLO", edit.getLabel());
}

TEST(EditFieldTest, BackspaceCharacterEndTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(strlen(edit.getLabel()));

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(strlen(edit.getLabel()), edit.getCaret());
	EXPECT_STREQ("Hello Worl", edit.getLabel());
}

TEST(EditFieldTest, BackspaceCharacterMiddleTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(3);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(2, edit.getCaret());
	EXPECT_STREQ("Helo World", edit.getLabel());
}

TEST(EditFieldTest, BackspaceCharacterStartTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(1);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(1, edit.getCaret());
	EXPECT_STREQ("Hello World", edit.getLabel());
}


TEST(EditFieldTest, DeleteCharacterEndTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(strlen(edit.getLabel()));

	kbd.keyDown(Neo::KEY_DELETE);
	event.update(0.0f);

	EXPECT_EQ(strlen(edit.getLabel()), edit.getCaret());
	EXPECT_STREQ("Hello World", edit.getLabel());
}

TEST(EditFieldTest, DeleteCharacterMiddleTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(2);

	kbd.keyDown(Neo::KEY_DELETE);
	event.update(0.0f);

	EXPECT_EQ(2, edit.getCaret());
	EXPECT_STREQ("Helo World", edit.getLabel());
}

TEST(EditFieldTest, MoveCaretLeftTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(2);

	kbd.keyDown(Neo::KEY_LEFT_ARROW);
	event.update(0.0f);
	event.update(0.0f);
	event.update(0.0f);

	EXPECT_EQ(1, edit.getCaret());
}

TEST(EditFieldTest, MoveCaretRightTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	Neo2D::Gui::EditField edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(9);

	kbd.keyDown(Neo::KEY_RIGHT_ARROW);
	event.update(0.0f);
	event.update(0.0f);
	event.update(0.0f);

	EXPECT_EQ(11, edit.getCaret());
}