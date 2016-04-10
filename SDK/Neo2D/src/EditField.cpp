#include <EditField.h>
#include <Neo2DLevel.h>
#include <tinyutf8.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

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

		const Neo::Vector2 position = widget->getPosition() + offset;

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
				renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.0, 0.0, 0.0, 1), 0);
				renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,1), Vector4(0.4, 0.4, 0.4, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
				renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(1, 1, 1, 1), 0);
				break;

			default: break;
		}

		text->setText(widget->getLabel());
		draw2DText(text, position.x,
				   position.y + text->getSize() * 1.3f, 0.0f);
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
}

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
			if(getState() == WIDGET_HOVER)
				setState(WIDGET_NORMAL);
			break;

		case MOUSE_LEFT_CLICK:
			setState(WIDGET_SELECTED);
			break;
	}
}
