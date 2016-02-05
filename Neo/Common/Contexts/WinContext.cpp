//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#ifndef ANDROID
#include <Window/Window.h>
#else
#include <Window/AndroidWindow.h>
#endif

#include "WinContext.h"

using namespace Neo;

// screen
void WinContext::getScreenSize(unsigned int * width, unsigned int * height)
{
	NeoWindow * window = NeoWindow::getInstance();
	*width = window->getWidth();
	*height = window->getHeight();
}

Vector2 WinContext::getScreenSize()
{
	NeoWindow* window = NeoWindow::getInstance();
	return Vector2(window->getWidth(), window->getHeight());
}

// cursor
void WinContext::setCursorPosition(int x, int y)
{
	NeoWindow * window = NeoWindow::getInstance();
    window->setCursorPos(x, y);
}

void WinContext::hideCursor(void)
{
	NeoWindow * window = NeoWindow::getInstance();
	window->hideCursor();
}

void WinContext::showCursor(void)
{
	NeoWindow * window = NeoWindow::getInstance();
	window->showCursor();
}

// working directory
const char * WinContext::getWorkingDirectory(void)
{
	NeoWindow * window = NeoWindow::getInstance();
	return window->getWorkingDirectory();
}

// system tick
unsigned long WinContext::getSystemTick(void)
{
	NeoWindow * window = NeoWindow::getInstance();
	return window->getSystemTick();
}

void WinContext::setWindowTitle(const char* title)
{
	NeoWindow::getInstance()->setTitle(title);
}

void WinContext::setScreenSize(unsigned int width, unsigned int height)
{
	NeoWindow::getInstance()->resize(width, height);
}

