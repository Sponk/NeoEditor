/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiTextureFont.h
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


#ifndef _M_GUI_TEXTURE_FONT_H
#define _M_GUI_TEXTURE_FONT_H


class MGuiTextureFont
{
private:

	unsigned int m_texture;

	float m_space;
	float m_tabSpace;

public:

	 MGuiTextureFont(const char * filename);

public:

	inline void setTexture(unsigned int textureId){ m_texture = textureId; }

	unsigned int findPointedCharacter(const char * text, const MVector2 & position, float size, const MVector2 & point);

	MVector2 getMaxSize(const char * text, const float size);
	MVector2 getCharacterPosition(const char * text, const MVector2 & position, const float size, const unsigned int charId);

	// character spacing
	inline void setSpace(const float space){ m_space = space; }
	inline float getSpace(void){ return m_space; }

	inline void setTabSpace(const float tabSpace){ m_tabSpace = tabSpace; }
	inline float getTabSpace(void){ return m_tabSpace; }

	// drawing
	void draw(const char * text, const MVector2 & position, float size);
	void drawSelection(const char * text, const MVector2 & position, float size, unsigned int startId, unsigned int endId);
};	

#endif
