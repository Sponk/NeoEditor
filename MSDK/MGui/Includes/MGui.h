/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGui.h
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


#ifndef _M_GUI_H
#define _M_GUI_H


// variable types
enum M_VAR_TYPE
{
	M_VAR_NONE = 0,
	M_VAR_BOOL,
	M_VAR_INT,
	M_VAR_UINT,
	M_VAR_FLOAT,
	M_VAR_STRING
};

class MGuiButton;
class MGuiMenu;
class MGuiText;
class MGuiImage;
class MGuiEditText;
class MGuiSlide;
class MGuiWindow;

#include <stdio.h>
#include <vector>
#include <string>
#include <MCore.h>

#include "MWindow.h"
#include "MGuiEvents.h"
#include "MGuiTextureFont.h"
#include "MGui2d.h"
#include "MGuiSlide.h"
#include "MGuiWindow.h"
#include "MGuiButton.h"
#include "MGuiMenu.h"
#include "MGuiText.h"
#include "MGuiImage.h"
#include "MGuiEditText.h"
#include "MGuiFileBrowser.h"


// global functions
void set2dMode(MRenderingContext * render);


// gui texture
class MGuiTexture
{
public:

	std::string name;
	unsigned int textureId;
};

// MGui
class MGui
{
public:

	MGui(void);
	~MGui(void);

	// instance
	static MGui * getInstance(void)
	{
		static MGui m_instance;
		return &m_instance;
	};

private:

	// rendering context
	MRenderingContext * m_render;

	// textures
	std::vector <MGuiTexture *> m_textures;

	// fonts
	std::vector <MGuiTextureFont *> m_fonts;

	// windows
	std::vector <MGuiWindow *> m_windows;

public:

	// rendering context
	void setRenderingContext(MRenderingContext * render){ m_render = render; }
	MRenderingContext * getRenderingContext(void){ return m_render; }

	// windows
	void unHighLightAllWindows(void);

	// clear
	void clear(void);

	// editing
	bool isSomethingEditing(void);

	// textures
	bool loadTexture(const char * filename, unsigned int * textureId);

	// fonts
	inline void addFont(MGuiTextureFont * font){ m_fonts.push_back(font); }
	inline unsigned int getFontsNumber(void){ return m_fonts.size(); }
	inline MGuiTextureFont * getFont(unsigned int id){ return m_fonts[id]; }

	// windows
	inline void addWindow(MGuiWindow * window){ m_windows.push_back(window); }
	inline unsigned int getWindowsNumber(void){ return m_windows.size(); }
	inline MGuiWindow * getWindow(unsigned int id){ return m_windows[id]; }

	// drawing
	void draw(void);

	// events
	void onEvent(MWinEvent * windowEvent);
};

#endif
