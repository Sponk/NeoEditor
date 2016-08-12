#include <Button.h>
#include <CommonEvents.h>
#include <Neo2DLevel.h>

using namespace Neo;

// Can't test rendering
// LCOV_EXCL_START
class ButtonTheme : public Neo2D::Gui::Theme
{
	OText* text;
public:

	ButtonTheme()
	{
		text = Neo2D::Neo2DLevel::getInstance()->createText("assets/default.ttf", 12);
		text->setAlign(TEXT_ALIGN_CENTER);
		text->setColor(Vector4(0,0,0,1));
	}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{	
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();

		text->setText(widget->getLabel());
		const Neo::Vector2 position = widget->getPosition() + offset;

		switch(widget->getState())
		{
		case Neo2D::Gui::WIDGET_HOVER:
			//renderer->drawColoredQuad(widget->getPosition(), widget->getSize(), Vector4(0.5, 0.5, 0.5, 1), 0);
			//break;

		case Neo2D::Gui::WIDGET_NORMAL:
			renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
			renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
			renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
			renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);

			renderer->drawText2D(text, position.x  + 0.5f * widget->getSize().x,
					 				position.y + text->getSize() * 1.25, 0.0f);

			break;

		case Neo2D::Gui::WIDGET_SELECTED:
			renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.0, 0.0, 0.0, 1), 0);
			renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,1), Vector4(0.4, 0.4, 0.4, 1), 0);
			renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
			renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);

			renderer->drawText2D(text, position.x  + 0.51f * widget->getSize().x,
								 position.y + text->getSize() * 1.25 + 0.1f * widget->getSize().y, 0.0f);

			break;

		default: break;
		}
	}
};
// LCOV_EXCL_STOP

Neo2D::Gui::Button::Button(int x, int y, unsigned int w, unsigned int h,
								const char* label, const shared_ptr<Neo2D::Object2D>& parent, const shared_ptr<Theme>& theme) :
	Neo2D::Gui::Widget(x, y, w, h, label, parent, (theme == nullptr) ? std::make_shared<ButtonTheme>() : theme)
{}

void Neo2D::Gui::Button::init()
{
	registerEvent(std::make_shared<MouseLeftReleaseEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(std::make_shared<MouseLeftClickEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(std::make_shared<MouseOverEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(std::make_shared<MouseLeaveEvent>(shared_from_this(), nullptr, nullptr));
}

bool Neo2D::Gui::Button::handle(const Neo2D::Gui::Event& event)
{
	switch(event.getType())
	{
	case MOUSE_OVER:
		setState(WIDGET_HOVER);
		return true;

	case MOUSE_LEAVE:
		setState(WIDGET_NORMAL);
		return true;

	case MOUSE_LEFT_CLICK:
		setState(WIDGET_SELECTED);
		return true;

	case MOUSE_LEFT_RELEASE:
		setState(WIDGET_HOVER);
		doCallback();
		return true;
	}

	return false;
}
