#include <Label.h>
#include <NeoEngine.h>
#include <Neo2DLevel.h>

using namespace Neo;

class LabelTheme : public Neo2D::Gui::Theme
{
	OText* text;
public:

	LabelTheme()
	{
		text = Neo2D::Neo2DLevel::getInstance()->createText("assets/default.ttf", 12);
	}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		const Neo::Vector2 position = widget->getPosition() + offset;

		text->setColor(static_cast<Neo2D::Gui::Label*>(widget)->getColor());
		text->setText(widget->getLabel());
		draw2DText(text, position.x,
				   position.y + text->getSize() * 0.7, 0.0f);
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

Neo2D::Gui::Label::Label(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent)
	: Widget(x, y, w, h, label, parent, make_shared<LabelTheme>()), m_color(1,1,1,1) {}
