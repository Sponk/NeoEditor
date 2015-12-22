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
	
	//render->set2D(m_width, m_height);
	//renderContext->setViewport(offset.x, res.y - (offset.y + m_height), m_width, m_height);
	renderContext->setScissor(offset.x, res.y - (offset.y + m_height), m_width - SCROLL_BAR_WIDTH, m_height);
	renderContext->enableScissorTest();
	
	for(Widget* w : m_content)
	{
		w->draw(curValue);
	}
	
	renderContext->disableScissorTest();

	m_horizontal->draw(Vector2());
	m_vertical->draw(Vector2());
	// renderContext->disableStencilTest();

	//render->set2D(res.x, res.y);
	//renderContext->setViewport(0,0, res.x, res.y);
}

void ScrollPane::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	Vector2 offset = m_offset + getPosition();
	unsigned int x = 0;
	unsigned int y = 0;
	Vector2 res = system->getScreenSize();
	x = input->getAxis("MOUSE_X") * res.x;
	y = input->getAxis("MOUSE_Y") * res.y;

	positionScrollBars();
	
	float scrollWheel = input->getAxis("MOUSE_WHEEL") * 10.0;
	Vector2 curValue = calculateValue();

	m_horizontal->setOffset(m_offset);
	m_vertical->setOffset(m_offset);
	
	m_horizontal->update();

	if (x >= offset.x && x <= offset.x + m_width
		&& y >= offset.y && y <= offset.y + m_height)
		m_vertical->setValue(m_vertical->getValue() - scrollWheel);

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

	return max + Vector2(5,15); // Add a small margin so there is some border around the content
}
