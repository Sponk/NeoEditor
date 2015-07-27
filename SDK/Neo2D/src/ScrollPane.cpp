#include <NeoEngine.h>
#include <ScrollPane.h>
#include <Render.h>
#include <cmath>

using namespace Neo;
using namespace Neo2D;
using namespace Neo2D::Gui;

ScrollPane::~ScrollPane()
{
	for(Widget* w : m_content)
		SAFE_DELETE(w);

	m_content.clear();
}

void ScrollPane::draw(Vector2 offset)
{
	NeoEngine* engine = NeoEngine::getInstance();
	Render* render = Render::getInstance();
	RenderingContext* renderContext = engine->getRenderingContext();
	SystemContext* system = engine->getSystemContext();
	Vector2 curValue = calculateValue();

	Vector2 res = system->getScreenSize();
	offset += m_offset + getPosition();
	
	// Draw body
	// renderContext->enableStencilTest();
	
	render->drawColoredQuad(m_x, m_y, m_width, m_height, Vector4(1, 1, 1, 1), 0.0);
	m_horizontal->draw(Vector2());
	m_vertical->draw(Vector2());
	
	//render->set2D(m_width, m_height);
	//renderContext->setViewport(offset.x, res.y - (offset.y + m_height), m_width, m_height);
	renderContext->setScissor(offset.x, res.y - (offset.y + m_height), m_width, m_height);
	renderContext->enableScissorTest();
	
	for(Widget* w : m_content)
	{
		w->draw(curValue);
	}
	
	renderContext->disableScissorTest();
	// renderContext->disableStencilTest();

	//render->set2D(res.x, res.y);
	//renderContext->setViewport(0,0, res.x, res.y);
}

void ScrollPane::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	Vector2 sz = calculateContentSize();
	m_horizontal->setRange(Vector2(m_width, MAX(m_width, sz.x)));
	m_vertical->setRange(Vector2(m_height, MAX(m_height, sz.y)));
	
	Vector2 curValue = calculateValue();

	m_horizontal->setOffset(m_offset);
	m_vertical->setOffset(m_offset);
	
	m_horizontal->update();
	m_vertical->update();

	for(Widget* w : m_content)
	{
		w->setOffset(curValue + m_offset);
		w->update();
	}
}

Vector2 ScrollPane::calculateContentSize()
{
	Vector2 max;
	for(Widget* w : m_content)
	{
		Vector2 pos = w->getPosition();
		Vector2 sz = w->getSize();

		pos += sz;
		if(pos.x > max.x)
			max.x = pos.x;

		if(pos.y > max.y)
			max.y = pos.y;		   
	}

	return max + Vector2(5,5); // Add a small margin so there is some border around the content
}
