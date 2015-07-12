#include <NeoEngine.h>
#include <Window.h>
#include <Render.h>

using namespace Neo;
using namespace Neo2D;
using namespace Neo2D::Gui;

Window::~Window()
{
	Container::~Container();
}

void Window::draw()
{
	Render* render = Render::getInstance();

	if (m_labelText == NULL)
	{
		m_labelText = render->createText(Neo2DEngine::getInstance()->getDefaultFont(),
			m_fontSize);
		m_labelText->setAlign(TEXT_ALIGN_LEFT);
	}

	m_labelText->setText(m_label.c_str());

	// Draw window title bar
	render->drawColoredQuad(m_x, m_y-20, m_width, 20, Vector4(0.2, 0.2, 0.2, 1.0), 0.0);
	render->drawText(m_labelText, m_x + 5, m_y-5);

	// Draw body
	render->drawColoredQuad(m_x, m_y, m_width, m_height, Vector4(0.5, 0.5, 0.5, 1.0), 0.0);

	Container::draw();
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

	if (input->isKeyPressed("MOUSE_BUTTON_LEFT") &&
		x >= m_x && x <= m_x + m_width
		&& y <= m_y
		&& y >= m_y - 20 || m_state == WINDOW_GRABBED_STATE)
	{
		m_x = m_x + dx;
		m_y = m_y + dy;

		m_state = WINDOW_GRABBED_STATE;
	}
	
	if (!input->isKeyPressed("MOUSE_BUTTON_LEFT")
		&& m_state == WINDOW_GRABBED_STATE)
	{
		m_state = WINDOW_SELECTED_STATE;
	}

	Container::update();
}
