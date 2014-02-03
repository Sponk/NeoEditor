/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGui.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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


#include <MLoaders/MDevILLoader.h>
#include "../Includes/MGui.h"
#include <MGui.h>

// global functions
void set2dMode(MRenderingContext * render)
{
	MWindow * window = MWindow::getInstance();

	render->setViewport(0, 0, window->getWidth(), window->getHeight());

	// set ortho projection
	render->setMatrixMode(M_MATRIX_PROJECTION);
	render->loadIdentity();

	render->setOrthoView(0, (float)window->getWidth(), (float)window->getHeight(), 0, 1.0f, -1.0f);

	render->setMatrixMode(M_MATRIX_MODELVIEW);
	render->loadIdentity();
}


// MGui
MGui::MGui(void):
m_render(NULL)
{
}

MGui::~MGui(void)
{
	clear();
}

void MGui::clear(void)
{
	unsigned int i;

	// clear textures
	unsigned int tSize = m_textures.size();
	for(i=0; i<tSize; i++)
	{
		m_render->deleteTexture(&m_textures[i]->textureId);
		SAFE_DELETE(m_textures[i]);
	}

	m_textures.clear();

	// clear fonts
	unsigned int fSize = m_fonts.size();
	for(i=0; i<fSize; i++)
		SAFE_DELETE(m_fonts[i]);

	m_fonts.clear();

	// clear windows
	unsigned int wSize = m_windows.size();
	for(i=0; i<wSize; i++)
		SAFE_DELETE(m_windows[i]);

	m_windows.clear();
}

bool MGui::loadTexture(const char * filename, unsigned int * textureId)
{
	if(! filename)
		return false;

	unsigned int i;
	unsigned int tSize = m_textures.size();
	for(i=0; i<tSize; i++)
	{
		MGuiTexture * texture = m_textures[i];
        if (!texture)
            continue;
		if(strcmp(filename, texture->name.c_str()) == 0)
		{
			*textureId = texture->textureId;
			return true;
		}
	}

	MImage image;
	if(M_loadImage(filename, &image))
	{
		MGuiTexture * texture = new MGuiTexture();

		m_render->createTexture(textureId);
		m_render->bindTexture(*textureId);
		m_render->sendTextureImage(&image, false, true, false);
		m_render->setTextureUWrapMode(M_WRAP_CLAMP);
		m_render->setTextureVWrapMode(M_WRAP_CLAMP);

		texture->textureId = *textureId;
		texture->name = filename;
		m_textures.push_back(texture);
		return true;
	}

	return false;
}

void MGui::draw(void)
{
	unsigned int i;
	unsigned int wSize = m_windows.size();

	// windows
	for(i=0; i<wSize; i++)
		m_windows[i]->draw();

	// menus
	for(i=0; i<wSize; i++)
		m_windows[i]->drawWindowMenus();
}

bool MGui::isSomethingEditing(void)
{
	unsigned int i;

	// windows
	unsigned int wSize = m_windows.size();
	for(i=0; i<wSize; i++)
	{
		if(m_windows[i]->isVisible() && m_windows[i]->isSomethingEditing())
			return true;
	}

	return false;
}

void MGui::unHighLightAllWindows(void)
{
	unsigned int i;
	unsigned int wSize = m_windows.size();

	// windows
	for(i=0; i<wSize; i++)
		m_windows[i]->setHighLight(false);
}

void MGui::onEvent(MWinEvent * windowEvent)
{
	int i, j;
	int wSize = (int)m_windows.size();

	if(wSize < 1)
		return;

	// menus windows
	for(i=0; i<wSize; i++)
	{
		if(! m_windows[i]->isVisible())
			continue;

		int result = m_windows[i]->onWindowMenusEvent(windowEvent);
		if(result != 0)
			return;
	}

	// windows
	for(i=wSize-1; i>=0; i--)
	{
		if(! m_windows[i]->isVisible())
			continue;

		m_windows[i]->onEvent(windowEvent);

		if(windowEvent->type == MWIN_EVENT_MOUSE_MOVE && m_windows[i]->isHighLight()) // priority window
		{
			for(j=i-1; j>=0; j--)
			{
				if(! m_windows[j]->isVisible())
					continue;

				m_windows[j]->onEvent(windowEvent);
				m_windows[j]->setHighLight(false);
			}

			return;
		}
	}
}
