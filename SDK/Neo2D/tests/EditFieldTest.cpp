#include <gtest/gtest.h>
#include <EditField.h>

class EditTester : public Neo2D::Gui::EditField
{
public:
	using Neo2D::Gui::EditField::setState;
	EditTester(int x,
			   int y,
			   unsigned int w,
			   unsigned int h,
			   const char* label,
			   const shared_ptr<Neo2D::Object2D>& parent,
			   const shared_ptr<Neo2D::Gui::Theme>& theme)
		: EditField(x, y, w, h, label, parent, theme)
	{ }
};

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

	EditTester edit(0,0,0,0,nullptr,nullptr,nullptr);
	Neo2D::Gui::CharacterInputEvent event(edit, nullptr, nullptr);

	// Select widget
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

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

	EditTester edit(0,0,0,0,"HELLO",nullptr,nullptr);
	Neo2D::Gui::CharacterInputEvent event(edit, nullptr, nullptr);

	edit.setState(Neo2D::Gui::WIDGET_SELECTED);
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

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(strlen(edit.getLabel()));
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

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

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(3);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(2, edit.getCaret());
	EXPECT_STREQ("Helo World", edit.getLabel());
}

TEST(EditFieldTest, BackspaceCharacterSecondPositionTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(1);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(0, edit.getCaret());
	EXPECT_STREQ("ello World", edit.getLabel());
}

TEST(EditFieldTest, BackspaceCharacterFirstPosition)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(0);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(0, edit.getCaret());
	EXPECT_STREQ("Hello World", edit.getLabel());
}

TEST(EditFieldTest, DeleteCharacterEndTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(strlen(edit.getLabel()));
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

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

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(2);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

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

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(2);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_LEFT_ARROW);
	event.update(0.0f);
	event.update(0.0f);
	event.update(0.0f);

	EXPECT_EQ(0, edit.getCaret());
}

TEST(EditFieldTest, MoveCaretRightTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,"Hello World",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(9);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_RIGHT_ARROW);
	event.update(0.0f);
	event.update(0.0f);
	event.update(0.0f);

	EXPECT_EQ(11, edit.getCaret());
}

TEST(EditFieldTest, MoveCaretRightUnicodeTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,u8"äöü",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(0);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_RIGHT_ARROW);
	event.update(0.0f);
	event.update(0.0f);
	event.update(0.0f);

	EXPECT_EQ(6, edit.getCaret());
}

TEST(EditFieldTest, MoveCaretLeftUnicodeTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,u8"aääöü",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(3);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_LEFT_ARROW);
	event.update(0.0f);
	event.update(0.0f);
	event.update(0.0f);

	EXPECT_EQ(0, edit.getCaret());
}

TEST(EditFieldTest, BackspaceCharacterMiddleUnicodeTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,u8"äöü",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(4);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(2, edit.getCaret());
	EXPECT_STREQ(u8"äü", edit.getLabel());
}

TEST(EditFieldTest, BackspaceCharacterEndUnicodeTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,u8"aöü",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(5);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);

	EXPECT_EQ(3, edit.getCaret());
	EXPECT_STREQ(u8"aö", edit.getLabel());
}

TEST(EditFieldTest, BackspaceCharacterStartUnicodeTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,u8"äöü",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(2);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_BACKSPACE);
	event.update(0.0f);
	event.update(0.0f);

	EXPECT_EQ(0, edit.getCaret());
	EXPECT_STREQ(u8"öü", edit.getLabel());
}

TEST(EditFieldTest, DeleteCharacterMiddleUnicodeTest)
{
	Neo::InputContextDummy input;
	Neo::Keyboard& kbd = input.getKeyboard();
	Neo::NeoEngine::getInstance()->setInputContext(&input);

	EditTester edit(0,0,0,0,u8"äöü",nullptr,nullptr);
	Neo2D::Gui::KeyPressEvent event(edit, nullptr, nullptr);

	// Ensure initial caret position
	edit.setCaret(2);
	edit.setState(Neo2D::Gui::WIDGET_SELECTED);

	kbd.keyDown(Neo::KEY_DELETE);
	event.update(0.0f);

	EXPECT_EQ(2, edit.getCaret());
	EXPECT_STREQ(u8"äü", edit.getLabel());
}