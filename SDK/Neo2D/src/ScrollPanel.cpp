#include <ScrollPanel.h>

Neo2D::Gui::ScrollPanel::ScrollPanel(int x,
									 int y,
									 unsigned int w,
									 unsigned int h,
									 const shared_ptr<Object2D>& parent,
									 const shared_ptr<Theme>& knobtheme,
									 const shared_ptr<Theme>& background)
	: Container(x, y, w, h, parent),
	  verticalScroll(x,y,w,h,nullptr,SCROLLBAR_VERTICAL,knobtheme),
	  horizontalScroll(x,y,w,h,nullptr,SCROLLBAR_HORIZONTAL,background)
{}

void Neo2D::Gui::ScrollPanel::init()
{
	registerEvent(make_shared<MouseOverEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<MouseLeaveEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<MouseMiddleMoveEvent>(shared_from_this(), nullptr, nullptr));
}

void Neo2D::Gui::ScrollPanel::updateScrollbarPlacement()
{
	verticalScroll.setSize(Neo::Vector2(15, getSize().y));
	horizontalScroll.setSize(Neo::Vector2(getSize().x, 15));

	Neo::Vector2 content = calculateContentSize(lastOffset);
	verticalScroll.setRange(Neo::Vector2(0, content.y));
	horizontalScroll.setRange(Neo::Vector2(0, content.x));

	Neo::Vector2 vpos = Neo::Vector2(getPosition().x + getSize().x, getPosition().y);
	Neo::Vector2 hpos = Neo::Vector2(getPosition().x, getPosition().y + getSize().y);

	if (vpos != verticalScroll.getPosition())
	{
		verticalScroll.setPosition(vpos);
		verticalScroll.updateKnobPosition();
	}

	if (hpos != horizontalScroll.getPosition())
	{
		horizontalScroll.setPosition(hpos);
		horizontalScroll.updateKnobPosition();
	}
}

void Neo2D::Gui::ScrollPanel::draw(const Neo::Vector2& offset)
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	Neo::Renderer* renderer = engine->getRenderer();
	Neo::RenderingContext* render = engine->getRenderingContext();

	renderer->drawColoredQuad(getPosition(), getSize(), Neo::Vector4(1,1,1,1), 0.0f);

	Neo::Vector2 screen = engine->getSystemContext()->getScreenSize();
	Neo::Vector2 scissor = offset + getPosition();
	for(auto c : getChildren())
	{
		// Ensure the scissor is set correctly.
		// Prevents issues with nesting scroll panels and other widgets that change the scissor
		render->enableScissorTest();
		render->setScissor(scissor.x, screen.y - (scissor.y + getSize().y), getSize().x, getSize().y);

		c->draw(offset);
	}
	render->disableScissorTest();

	horizontalScroll.draw(offset);
	verticalScroll.draw(offset);
}

void Neo2D::Gui::ScrollPanel::update(float dt)
{
	updateFilter();
	Widget::update(dt);

	updateScrollbarPlacement();

	horizontalScroll.update(dt);
	verticalScroll.update(dt);

	Neo::Vector2 offset = Neo::Vector2(horizontalScroll.getValue(), verticalScroll.getValue());
	lastOffset -= offset;

	for(auto c : getChildren())
	{
		c->update(dt);
		c->setPosition(c->getPosition() + lastOffset);
	}

	lastOffset = offset;
	updateLayout(-lastOffset);
}
