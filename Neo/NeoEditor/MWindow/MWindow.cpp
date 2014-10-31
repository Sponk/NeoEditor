//========================================================================
// Copyright (c) 2014 Yannick Pflanzer <www.scary-squid.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#include <MWindow.h>
#include "NeoWindow.h"

MWindow::MWindow(void) :
	m_focus(true),
	m_active(true),
	m_fullscreen(false),
	m_width(0),
	m_height(0),
	m_colorBits(0),
	m_pointerEvent(NULL)
{

}

MWindow::~MWindow(void)
{

}

void MWindow::setCursorPos(int x, int y)
{
    NeoEditor::MWindow::getInstance()->setCursorPos(x, y);
}

void MWindow::hideCursor(void)
{
    NeoEditor::MWindow::getInstance()->hideCursor();
}

void MWindow::showCursor(void)
{
    NeoEditor::MWindow::getInstance()->showCursor();
}

void MWindow::setTitle(const char * title)
{
    NeoEditor::MWindow::getInstance()->setTitle(title);
}

void MWindow::setFullscreen(bool fullscreen)
{

}

void MWindow::sendEvents(MWinEvent * event)
{
    NeoEditor::MWindow::getInstance()->sendEvents(event);
}

bool MWindow::isMouseOverWindow(void)
{
    NeoEditor::MWindow::getInstance()->isMouseOverWindow();
}

unsigned long MWindow::getSystemTick(void)
{
    NeoEditor::MWindow::getInstance()->getSystemTick();
}

bool MWindow::onEvents(void)
{
    return NeoEditor::MWindow::getInstance()->onEvents();
}

void MWindow::swapBuffer(void)
{
    NeoEditor::MWindow::getInstance()->swapBuffer();
}

bool MWindow::create(const char * title, unsigned int width, unsigned int height, int colorBits, bool fullscreen)
{
    return NeoEditor::MWindow::getInstance()->create(title, width, height, colorBits, fullscreen);
}

void MWindow::sleep(double time)
{
    NeoEditor::MWindow::getInstance()->sleep(time);
}

int MWindow::addJoystick(int index)
{

}

int MWindow::removeJoystick(int id)
{

}

int MWindow::addGameController(int index)
{

}

int MWindow::removeGameController(int id)
{

}

void MWindow::messagebox(const char* content, const char* title)
{
    NeoEditor::MWindow::getInstance()->messagebox(content, title);
}

void MWindow::resize(unsigned int width, unsigned int height)
{
    NeoEditor::MWindow::getInstance()->resize(width, height);
}

