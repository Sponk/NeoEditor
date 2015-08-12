#include <NeoEngine.h>
#include <WindowManager.h>
#include <Window.h>

using namespace Neo;
using namespace Neo2D;
using namespace Neo2D::Gui;

WindowManager::~WindowManager()
{
	for(Widget* w : m_content)
		SAFE_DELETE(w);

	m_content.clear();
}

void WindowManager::draw()
{
	Container::draw();
}

void WindowManager::update()
{
	Container::update();
}

void WindowManager::addWindow(Window* window)
{
	window->setWindowManager(this);
	addWidget(window);
	selectWindow(window);
}

void WindowManager::selectWindow(Window* w)
{
	int idx = 0;
	for (int i = 0; i < m_content.size(); i++)
	{
		Widget* win = m_content[i];
		if (win == w)
		{
			m_selectedWindow = w;
			m_content.erase(m_content.begin() + idx);
			//m_content.insert(m_content.begin(), w);
			m_content.push_back(w);
		}

		idx++;
	}
}

bool WindowManager::isMouseOver()
{
	for(Widget* w : m_content)
	{
		if(w->isMouseOver())
			return true;
	}

	return false;
}
