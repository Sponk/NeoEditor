#include <NeoEngine.h>
#include <Window.h>
#include <Render.h>
#include <cmath>

using namespace Neo;
using namespace Neo2D;
using namespace Neo2D::Gui;

#define BORDER_WIDTH 3

Window::~Window()
{
	for(Widget* w : m_content)
		SAFE_DELETE(w);

	m_content.clear();
}

void Window::draw()
{
	NeoEngine* engine = NeoEngine::getInstance();
	Render* render = Render::getInstance();
	RenderingContext* renderContext = engine->getRenderingContext();
	SystemContext* system = engine->getSystemContext();

	Vector2 res = system->getScreenSize();

	if (m_labelText == NULL)
	{
		m_labelText = render->createText(Neo2DEngine::getInstance()->getDefaultFont(),
			m_fontSize);
		m_labelText->setAlign(TEXT_ALIGN_LEFT);
	}

	m_labelText->setText(m_label.c_str());

	renderContext->enableScissorTest();
	renderContext->setScissor(m_x, res.y - m_y, m_width, TITLE_HEIGHT);
	
	renderContext->clear(BUFFER_COLOR);

	// Draw window title bar
	render->drawColoredQuad(m_x, m_y - TITLE_HEIGHT, m_width, TITLE_HEIGHT, Vector4(0.2, 0.2, 0.2, 1.0), 0.0);
	render->drawText(m_labelText, m_x + 5,
					 m_y + 0.5 * m_labelText->getSize() -
					 0.5 * TITLE_HEIGHT);

	renderContext->disableScissorTest();

	// Draw body
	render->drawColoredQuad(m_x, m_y, m_width, m_height, Vector4(0.8, 0.8, 0.82, 1.0), 0.0);
	
	render->set2D(m_width, m_height);
	renderContext->setViewport(m_x, res.y - (m_y + m_height), m_width, m_height);

	Container::draw();
	renderContext->disableScissorTest();

	render->set2D(res.x, res.y);
	renderContext->setViewport(0,0, res.x, res.y);
	
	// Draw borders
	render->drawColoredQuad(m_x, m_y, BORDER_WIDTH, m_height+BORDER_WIDTH, Vector4(0.2, 0.2, 0.2, 1.0), 0.0);
	render->drawColoredQuad(m_x+m_width-BORDER_WIDTH, m_y, BORDER_WIDTH, m_height+BORDER_WIDTH, Vector4(0.2, 0.2, 0.2, 1.0), 0.0);
	render->drawColoredQuad(m_x, m_y+m_height, m_width, BORDER_WIDTH, Vector4(0.2, 0.2, 0.2, 1.0), 0.0);
}

void Window::update()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	unsigned int x = 0;
	unsigned int y = 0;
	Vector2 res = system->getScreenSize();
	x = input->getAxis("MOUSE_X") * res.x;
	y = input->getAxis("MOUSE_Y") * res.y;

	float dx = x - m_mx;
	float dy = y - m_my;

	m_mx = x;
	m_my = y;

	if (m_wm && input->onKeyDown("MOUSE_BUTTON_LEFT")
		&& fabs(dx) < 0.2 && fabs(dy) < 0.2)
	{
		if (containsPoint(x, y) && !m_wm->getSelectedWindow()->containsPoint(x,y))
		{
			m_wm->selectWindow(this);
		}
	}

	if (m_state != WINDOW_RESIZING_STATE
		&& input->onKeyDown("MOUSE_BUTTON_LEFT")
		&& x >= m_x && x <= m_x + m_width
		&& y <= m_y
		&& y >= m_y - TITLE_HEIGHT
		&& (m_wm && m_wm->getSelectedWindow() == this)
		|| m_state == WINDOW_GRABBED_STATE)
	{
		m_x = m_x + dx;
		m_y = m_y + dy;

		m_state = WINDOW_GRABBED_STATE;

		if(m_wm)
			m_wm->selectWindow(this);
	}

	if (input->onKeyUp("MOUSE_BUTTON_LEFT")
		&& (m_state == WINDOW_GRABBED_STATE ||
		   m_state == WINDOW_RESIZING_STATE))
	{
		//if(m_wm)
		//	m_wm->selectWindow(this);
			
		m_state = WINDOW_SELECTED_STATE;
	}

	// RESIZING
	if (m_state == WINDOW_SELECTED_STATE
		&& input->onKeyDown("MOUSE_BUTTON_LEFT")
		&& x >= m_x + m_width - TITLE_HEIGHT && x <= m_x + m_width
		&& y >= m_y + m_height - TITLE_HEIGHT && y <= m_y + m_height
		|| m_state == WINDOW_RESIZING_STATE
		&& (m_wm && m_wm->getSelectedWindow() == this))
	{
		m_width = m_width + dx;
		m_height = m_height + dy;

		// MIN SIZE!
		static int minSize = 50;
		if(m_width <= minSize)
			m_width = minSize;

		if(m_height <= minSize)
			m_height = minSize;

		m_state = WINDOW_RESIZING_STATE;
	}

	// Only update content if the window is selected
	if(m_wm->getSelectedWindow() == this
		&& m_state != WINDOW_GRABBED_STATE)

		Container::update();
}

bool Window::isMouseOver()
{
	NeoEngine* engine = NeoEngine::getInstance();
	SystemContext* system = engine->getSystemContext();
	InputContext* input = engine->getInputContext();

	unsigned int x = 0;
	unsigned int y = 0;
	Vector2 res = system->getScreenSize();
	x = input->getAxis("MOUSE_X") * res.x;
	y = input->getAxis("MOUSE_Y") * res.y;

	unsigned int wx = m_offset.x + m_x;
	unsigned int wy = m_offset.y + m_y;

	return isVisible() && (x >= wx && x <= wx + m_width
			&& (y + TITLE_HEIGHT) >= wy && y <= wy + m_height);
}
