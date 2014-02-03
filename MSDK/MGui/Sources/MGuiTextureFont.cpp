/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MGuiTextureFont.cpp
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


#include <stdio.h>
#include "../Includes/MGui.h"

MGuiTextureFont::MGuiTextureFont(const char * filename):
m_texture(0),
m_space(0.5f),
m_tabSpace(2.0f)
{
	MGui::getInstance()->loadTexture(filename, &m_texture);
}

MVector2 MGuiTextureFont::getCharacterPosition(const char * text, const MVector2 & position, const float size, const unsigned int charId)
{
	MVector2 pos;
	float xc = position.x;
	float yc = position.y;

	unsigned int i;
	unsigned int tSize = strlen(text);
	for(i=0; i<charId && i<tSize; i++)
	{
		if(text[i] == '\n')	// return
		{
			yc += size;
			xc = position.x;
		}
		else if(text[i] == '	') // tab
		{
			xc += size * getTabSpace();
		}
		else
		{
			xc += size * getSpace(); // move to next character
		}
	}

	pos.x = xc;
	pos.y = yc;

	return pos;
}

MVector2 MGuiTextureFont::getMaxSize(const char * text, const float size)
{
	MVector2 max;
	float xc=0, yc=0;

	max.set(0, 0);

	unsigned int i;
	unsigned int tSize = strlen(text);
	for(i=0; i<tSize; i++)
	{
		if(text[i] == '\n') // return
		{
			yc += size;
			xc = 0;

			if(yc > max.y) max.y = yc;
		}
		else if(text[i] == '	') // tab
		{
			xc += size * getTabSpace(); //move to next character

			if(xc > max.x) max.x = xc;
		}
		else
		{
			xc += size * getSpace(); //move to next character

			if(xc > max.x) max.x = xc;
		}
	}

	max.x += size * getSpace();
	max.y += size;

	return max;
}

unsigned int MGuiTextureFont::findPointedCharacter(const char * text, const MVector2 & position, const float size, const MVector2 & point)
{
	float xc=position.x, yc=position.y;
	float halfSpace = getSpace() * 0.50001f;
	float halfTabSpace = getTabSpace() - halfSpace;

	unsigned int tSize = strlen(text);

	// if out (up) of the first line
	if(point.y <= yc){
		return 0;
	}

	// if out (left) of the first line
	if(point.x <= xc)
	if(point.y <= (yc + size) && point.y >= yc){
		return 0;
	}

	unsigned int i;
	for(i=0; i<tSize; i++)
	{
		if(point.x < (xc + (size*0.5001f)) && point.x >= (xc + (size*0.5001f) - (size*getSpace())))
		if(point.y < (yc + size) && point.y >= yc){
			return i;
		}

		if(text[i] == '\n')	// return
		{
			// if out (right) of a line
			if(point.x >= (xc + (size * halfSpace)))
			if(point.y <= (yc + size) && point.y >= yc){
				return i;
			}

			yc += size;
			xc = position.x;

			// if out (left) of a line
			if(point.x <= xc)
			if(point.y <= (yc + size) && point.y >= yc){
				return i + 1;
			}
		}
		else if(text[i] == '	') // tab
		{
			if(point.x < (xc + (size * halfTabSpace)) && point.x >= (xc + (size*0.5001f) - (size*getSpace())))
			if(point.y < (yc + size) && point.y >= yc){
				return i;
			}

			if(point.x < (xc + (size * getTabSpace()) + (size*0.5001f) - (size*getSpace())) && point.x >= (xc + (size * halfTabSpace)))
			if(point.y < (yc + size) && point.y >= yc){
				if((i+1) < tSize)
					return i+1;
			}

			xc += size * getTabSpace();
		}
		else
		{
			xc += size * getSpace(); // move to next character
		}
	}

	return (unsigned int)tSize;
}

void MGuiTextureFont::draw(const char * text, const MVector2 & position, float size)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	MVector2 g_vertices[4];
	MVector2 g_texCoords[4];

	float xc=position.x, yc=position.y, u=0, v=0;

	render->disableNormalArray();
	render->disableColorArray();
	render->enableVertexArray();
	render->enableTexCoordArray();

	render->setVertexPointer(M_FLOAT, 2, g_vertices);
	render->setTexCoordPointer(M_FLOAT, 2, g_texCoords);

	render->bindTexture(m_texture);

	unsigned int i;
	unsigned int textLength = strlen(text);
	for(i=0; i<textLength; i++)
	{
		if(text[i] == '\n')	// return
		{
			yc += size;
			xc = position.x;
		}
		else if(text[i] == '	') // tab
		{
			xc += size * getTabSpace();
		}
		else
		{
			u=float((unsigned char)text[i]%16)/16.0f; //u position of character
			v=float((unsigned char)text[i]/16)/16.0f; //v position of character

			g_texCoords[0] = MVector2(u, v+0.0625f);
			g_vertices[0] = MVector2(xc, yc+size);

			g_texCoords[1] = MVector2(u+0.0625f, v+0.0625f);
			g_vertices[1] = MVector2(xc+size, yc+size);

			g_texCoords[3] = MVector2(u+0.0625f, v+0.001f);
			g_vertices[3] = MVector2(xc+size, yc);

			g_texCoords[2] = MVector2(u, v+0.001f);
			g_vertices[2] = MVector2(xc, yc);

			render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);

			xc += size * getSpace(); //move to next character
		}
	}
}

void MGuiTextureFont::drawSelection(const char * text, const MVector2 & position, float size, unsigned int startId, unsigned int endId)
{
	MRenderingContext * render = MGui::getInstance()->getRenderingContext();

	if(startId == endId)
		return;

	MVector2 g_vertices[4];

	render->disableNormalArray();
	render->disableColorArray();
	render->disableTexCoordArray();
	render->enableVertexArray();

	render->setVertexPointer(M_FLOAT, 2, g_vertices);

	unsigned int start;
	unsigned int end;

	if(endId > startId)
	{
		start = startId;
		end = endId;
	}
	else
	{
		start = endId;
		end = startId;
	}

	render->disableTexture();

	MVector2 startPosition = getCharacterPosition(text, position, size, start);
	float xc = startPosition.x;
	float yc = startPosition.y;

	float offset = (size - (size*getSpace()))*0.5f;

	// left quad
	g_vertices[0] = MVector2(xc+offset, yc);
	g_vertices[1] = MVector2(xc+offset, yc+size);

	unsigned int i;
	unsigned int textLength = strlen(text);
	for(i=start; i<end && i<textLength; i++)
	{
		if(text[i] == '\n')	// return
		{
			// right quad
			{
				g_vertices[3] = MVector2(xc+size*0.5f, yc+size);
				g_vertices[2] = MVector2(xc+size*0.5f, yc);
				render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
			}

			yc += size;
			xc = position.x;

			// left quad
			{
				g_vertices[0] = MVector2(xc+offset, yc);
				g_vertices[1] = MVector2(xc+offset, yc+size);
			}
		}
		else if(text[i] == '	') // tab
		{
			xc += size * getTabSpace();

			if(i+1 == end || i+1 == textLength)
			{
				// right quad
				g_vertices[3] = MVector2(xc+offset, yc+size);
				g_vertices[2] = MVector2(xc+offset, yc);
				render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
			}
		}
		else
		{
			xc += size * getSpace(); //move to next character

			if(i+1 == end || i+1 == textLength)
			{
				// right quad
				g_vertices[3] = MVector2(xc+offset, yc+size);
				g_vertices[2] = MVector2(xc+offset, yc);
				render->drawArray(M_PRIMITIVE_TRIANGLE_STRIP, 0, 4);
			}
		}
	}
}
