#include <EditField.h>
#include <tinyutf8.h>

using namespace Neo2D;
using namespace Gui;

void Neo2D::Gui::EditField::handle(const Event& e)
{
	switch(e.getType())
	{
		case CHARACTER_INPUT: {
			auto keypress = static_cast<const CharacterInputEvent&>(e);
			std::string str(getLabel());
			unsigned int character = keypress.getCharacter();

			string postfix = str.substr(getCaret());
			str.erase(getCaret());

			str = str + (const char*) &character + postfix;

			m_caret += strlen((const char*) &character);
			setLabel(str.c_str());
		}
			break;

		case KEY_PRESSED:
		{
			auto keypress = static_cast<const KeyPressEvent&>(e);
			std::string str(getLabel());

			switch (keypress.getKey())
			{
				case Neo::KEY_BACKSPACE:
				{
					if (str.length() >= 1 && getCaret() > 1)
					{
						auto endDelete = str.begin() + getCaret();
						str.erase(endDelete - 1, endDelete);
						setCaret(getCaret() - 1);
					}
				}
					break;

				case Neo::KEY_DELETE:
				{
					if (str.length() >= 1 && getCaret() < str.length())
					{
						auto endDelete = str.begin() + getCaret();
						str.erase(endDelete, endDelete + 1);
					}
				}
					break;

				case Neo::KEY_LEFT_ARROW:
					if(m_caret > 1)
						m_caret--;
					break;

				case Neo::KEY_RIGHT_ARROW:
					if(m_caret < strlen(getLabel()))
						m_caret++;
					break;
			}
			setLabel(str.c_str());

		}
			break;

		case MOUSE_OVER:
			setState(WIDGET_HOVER);
			break;

		case MOUSE_LEAVE:
			setState(WIDGET_NORMAL);
			break;

		case MOUSE_LEFT_CLICK:
			setState(WIDGET_SELECTED);
			break;
	}
}
