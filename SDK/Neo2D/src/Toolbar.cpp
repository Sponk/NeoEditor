#include <Toolbar.h>

using namespace Neo2D;
using namespace Gui;
using namespace Neo;

// Can't test rendering
// LCOV_EXCL_START
class ToolbarTheme : public Neo2D::Gui::Theme
{
public:

	ToolbarTheme() {}

	virtual void draw(Neo2D::Gui::Widget* widget, const Neo::Vector2& offset)
	{
		NeoEngine* engine = NeoEngine::getInstance();
		Renderer* renderer = engine->getRenderer();

		const Neo::Vector2 position = widget->getPosition() + offset;

		renderer->drawColoredQuad(position, widget->getSize(), Vector4(0.1, 0.1, 0.1, 1), 0);
		renderer->drawColoredQuad(position, widget->getSize() - Vector2(1,2), Vector4(1, 1, 1, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(2,2), Vector4(0.3, 0.3, 0.3, 1), 0);
		renderer->drawColoredQuad(position + Vector2(1,1), widget->getSize() - Vector2(3,3), Vector4(0.8, 0.8, 0.8, 1), 0);
	}
};

Toolbar::Toolbar(int x, int y, unsigned int w, unsigned int h, const shared_ptr<Object2D>& parent, const shared_ptr<Theme>& theme)
	: Container(x, y, w, h, parent)
{
	setTheme(theme == nullptr ? make_shared<ToolbarTheme>() : theme);
}

// LCOV_EXCL_STOP
