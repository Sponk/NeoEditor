#include <EditField.h>
#include <Neo2DLevel.h>
#include <utf8.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

static unsigned int characterSize(unsigned int c)
{
	if (c <= 0x7F)
		return 1;
	else if (c <= 0x7FF)
		return 2;
	else if (c <= 0xFFFF)
		return 3;
	else if (c <= 0x10FFFF)
		return 4;
}

class EditFieldTheme : public Neo2D::Gui::Theme
{
	OText* text;
public:

	EditFieldTheme()
	{
		text = Neo2D::Neo2DLevel::getInstance()->createText("assets/default.ttf", 12);
		text->setColor(Vector4(0,0,0,1));
	}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();
		EditField* edit = static_cast<EditField*>(widget);

		const Neo::Vector2 position = widget->getPosition() + offset + Vector2(2,0);
		Box3d* box = text->getBoundingBox();

		text->setText(widget->getLabel());
		Vector2 cursorPos = calculateCursorPos(text, edit->getCaret()) + offset + edit->getPosition();
		float textOffset = cursorPos.x;

		// Calculate overflow offset
		if (textOffset <= edit->getSize().x) textOffset = 0.0f; // If we do not overflow, simply ignore
		else textOffset -= edit->getSize().x; // Calculate the new position relative to the right edge

		switch(widget->getState())
		{
			case Neo2D::Gui::WIDGET_HOVER:
			case Neo2D::Gui::WIDGET_NORMAL:
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(1, 1, 1, 1), 0);
				break;

			case Neo2D::Gui::WIDGET_SELECTED:
			{
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.0, 0.0, 0.0, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1, 1), Vector4(0.4, 0.4, 0.4, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1, 1),  widget->getSize() - Vector2(2, 2),
										  Vector4(0.3, 0.3, 0.3, 1),
										  0);
				renderer->drawColoredQuad(position + Vector2(1, 1),
										  widget->getSize() - Vector2(3, 3),
										  Vector4(1, 1, 1, 1),
										  0);

				renderer->drawColoredQuad(Vector2(cursorPos.x - textOffset + 3, cursorPos.y + 0.25 * text->getSize()),
										  Vector2(2.0, 0.75*text->getSize()),
										  text->getColor() + Vector4(0.1,0.1,0.1,0.0), 0.0f);

			}
				break;

			default: break;
		}

		// FIXME: SHOULD NOT BE NECESSARY TO USE A RenderingContext!
		RenderingContext* renderContext = engine->getRenderingContext();
		Vector2 screen = engine->getSystemContext()->getScreenSize();

		renderContext->enableScissorTest();
		renderContext->setScissor(position.x, screen.y - (position.y + edit->getSize().y), edit->getSize().x, edit->getSize().y);

		draw2DText(text, position.x - textOffset,
						 position.y + 0.5 * edit->getSize().y,
						 0.0f);

		renderContext->disableScissorTest();
	}

	Vector2 calculateCursorPos(OText* text, int pos) const
	{
		Vector2 length;
		Font* font = text->getFontRef()->getFont();
		map<unsigned int, Character>* chars = font->getCharacters();

		std::string s = text->getText();
		auto iter = s.begin();
		int i = 0;

		while(iter != s.end() && (i < pos || pos == -1))
		{
			const unsigned int character = utf8::next(iter, s.end());
			length.x += ((*chars)[character]).getXAdvance();

			if(character == '\n')
			{
				length.x = 0;
				length.y++;
			}
			i += characterSize(character);
		}

		return length * text->getSize();
	}

	void draw2DText(OText* text, float x, float y, float rotation)
	{
		RenderingContext* renderContext =
			NeoEngine::getInstance()->getRenderingContext();

		text->setPosition(Vector3(floor(x), floor(y), 0));
		text->setRotation(Quaternion(0, 0, rotation));
		text->updateMatrix();

		renderContext->pushMatrix();
		renderContext->multMatrix(text->getMatrix());

		NeoEngine::getInstance()->getRenderer()->drawText(text);
		renderContext->popMatrix();
	}
};

Neo2D::Gui::EditField::EditField(int x,
								 int y,
								 unsigned int w,
								 unsigned int h,
								 const char* label,
								 const shared_ptr<Object2D>& parent,
								 const shared_ptr<Theme>& theme)
	: Widget(x, y, w, h, label, parent, (theme == nullptr) ? make_shared<EditFieldTheme>() : theme),
	  m_caret(0)
{
	registerEvent(make_shared<MouseOverEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<MouseLeaveEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<MouseLeftClickEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<CharacterInputEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<KeyPressEvent>(*this, nullptr, nullptr));
	registerEvent(make_shared<MouseDeselectEvent>(*this, nullptr, nullptr));
}

void Neo2D::Gui::EditField::handle(const Event& e)
{
	switch(e.getType())
	{
		case CHARACTER_INPUT: {

			if(getState() != WIDGET_SELECTED)
				break;

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

			if(getState() != WIDGET_SELECTED)
				break;

			switch (keypress.getKey())
			{
				case Neo::KEY_BACKSPACE:
				{
					// Don't go back if we don't have to
					if(m_caret == 0) break;

					auto i = str.begin() + m_caret;
					int codepoint = utf8::prior(i, str.begin());

					unsigned int size = characterSize(codepoint);

					if (str.length() >= size && m_caret >= size)
					{
						auto endDelete = i + size;
						str.erase(i, endDelete);
						m_caret -= size;
					}
				}
					break;

				case Neo::KEY_DELETE:
				{
					auto i = str.begin() + m_caret;
					if(i == str.end())
						break;

					size_t size = characterSize(utf8::peek_next(i, str.end()));
					if (str.length() >= size && getCaret() < str.length())
					{
						auto endDelete = i + size;
						str.erase(i, endDelete);
					}
				}
					break;

				case Neo::KEY_LEFT_ARROW:
				{
					// Don't go back if we don't have to
					if(m_caret == 0) break;

					auto i = str.begin() + m_caret;
					int codepoint = utf8::prior(i, str.begin());

					unsigned int size = characterSize(codepoint);
					if (m_caret >= size)
						m_caret -= size;
				}
					break;

				case Neo::KEY_RIGHT_ARROW:
				{
					auto i = str.begin() + m_caret;
					if(i == str.end())
						break;

					size_t size = characterSize(utf8::peek_next(i, str.end()));

					if (m_caret <= str.length() - size)
						m_caret += size;
				}
					break;
			}
			setLabel(str.c_str());

		}
			break;

		case MOUSE_OVER:
			if(getState() != WIDGET_SELECTED)
				setState(WIDGET_HOVER);
			break;

		case MOUSE_LEAVE:
			if(getState() == WIDGET_HOVER)
				setState(WIDGET_NORMAL);
			break;

		case MOUSE_LEFT_CLICK:
			setState(WIDGET_SELECTED);
			break;

		case MOUSE_DESELECT:
			setState(WIDGET_NORMAL);
			break;
	}
}
