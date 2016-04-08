#include <Button.h>
#include <MouseEvents.h>
#include <Neo2DLevel.h>

using namespace Neo;

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

		switch(widget->getState())
		{
		case Neo2D::Gui::WIDGET_HOVER:
			//renderer->drawColoredQuad(widget->getPosition(), widget->getSize(), Vector4(0.5, 0.5, 0.5, 1), 0);
			//break;

		case Neo2D::Gui::WIDGET_NORMAL:
			renderer->drawColoredQuad(widget->getPosition(), widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
			renderer->drawColoredQuad(widget->getPosition(), widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
			renderer->drawColoredQuad(widget->getPosition() + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
			renderer->drawColoredQuad(widget->getPosition() + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);

			draw2DText(text, widget->getPosition().x  + 0.5f * widget->getSize().x,
						   widget->getPosition().y + text->getSize() * 1.25, 0.0f);

			break;

		case Neo2D::Gui::WIDGET_SELECTED:
			renderer->drawColoredQuad(widget->getPosition(), widget->getSize(), Vector4(0.0, 0.0, 0.0, 1), 0);
			renderer->drawColoredQuad(widget->getPosition(), widget->getSize() - Vector2(1,1), Vector4(0.4, 0.4, 0.4, 1), 0);
			renderer->drawColoredQuad(widget->getPosition() + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
			renderer->drawColoredQuad(widget->getPosition() + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);

			draw2DText(text, widget->getPosition().x  + 0.51f * widget->getSize().x,
					   widget->getPosition().y + text->getSize() * 1.25 + 0.1f * widget->getSize().y, 0.0f);

			break;

		default: break;
		}
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

Neo2D::Gui::Button::Button(int x, int y, unsigned int w, unsigned int h, 
								const char* label, shared_ptr<Neo2D::Object2D> parent) :
	Neo2D::Gui::Widget(x, y, w, h, label, parent, std::make_shared<ButtonTheme>())
{
	registerEvent(std::make_shared<MouseLeftReleaseEvent>(*this, nullptr, this));
	registerEvent(std::make_shared<MouseLeftClickEvent>(*this, nullptr, this));
	registerEvent(std::make_shared<MouseOverEvent>(*this, nullptr, this));
	registerEvent(std::make_shared<MouseLeaveEvent>(*this, nullptr, this));
}

void Neo2D::Gui::Button::handle(const Neo2D::Gui::Event& event)
{
	switch(event.getType())
	{
	case MOUSE_OVER:
		setState(WIDGET_HOVER);
		break;

	case MOUSE_LEAVE:
		setState(WIDGET_NORMAL);
		break;

	case MOUSE_LEFT_CLICK:
		setState(WIDGET_SELECTED);
		break;

	case MOUSE_LEFT_RELEASE:
		setState(WIDGET_HOVER);
		doCallback();
		break;
	}
}