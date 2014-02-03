/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MImage.cpp
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


#include "../Includes/MCore.h"


MImage::MImage(void):
m_data(NULL),
m_dataType(M_UBYTE),
m_size(0),
m_width(0),
m_height(0),
m_components(0)
{}

MImage::~MImage(void)
{
	SAFE_FREE(m_data);
}

void MImage::clear(void * color)
{
	unsigned int i;
	unsigned int c;

	switch(m_dataType)
	{
	case M_UBYTE:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((unsigned char *)m_data)[i+c] = ((unsigned char *)color)[c];
		}
		break;
	case M_USHORT:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((unsigned short *)m_data)[i+c] = ((unsigned short *)color)[c];
		}
		break;
	case M_INT:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((int *)m_data)[i+c] = ((int *)color)[c];
		}
		break;
	case M_FLOAT:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((float *)m_data)[i+c] = ((float *)color)[c];
		}
		break;
	}
}

void MImage::create(M_TYPES dataType, unsigned int width, unsigned int height, unsigned int components)
{
	SAFE_FREE(m_data);

    m_width = width;
    m_height = height;
	m_components = components;

    m_size = m_components * m_width * m_height;

	switch(dataType)
	{
	default:
	case M_UBYTE:
		m_dataType = M_UBYTE;
		m_data = new unsigned char[m_size];
		break;
	case M_USHORT:
		m_dataType = M_USHORT;
		m_data = new unsigned short[m_size];
		break;
	case M_INT:
		m_dataType = M_INT;
		m_data = new int[m_size];
		break;
	case M_FLOAT:
		m_dataType = M_FLOAT;
		m_data = new float[m_size];
		break;
	}
}

void MImage::writePixel(unsigned int x, unsigned int y, void * color)
{
	unsigned int c;
	unsigned int pixelId = ((m_width * m_components) * y) + (x * m_components);

	switch(m_dataType)
	{
	case M_UBYTE:
		for(c=0; c<m_components; c++)
			((unsigned char *)m_data)[pixelId+c] = ((unsigned char *)color)[c];
		break;
	case M_USHORT:
		for(c=0; c<m_components; c++)
			((unsigned short *)m_data)[pixelId+c] = ((unsigned short *)color)[c];
		break;
	case M_INT:
		for(c=0; c<m_components; c++)
			((int *)m_data)[pixelId+c] = ((int *)color)[c];
		break;
	case M_FLOAT:
		for(c=0; c<m_components; c++)
			((float *)m_data)[pixelId+c] = ((float *)color)[c];
		break;
	}
}

void MImage::readPixel(unsigned int x, unsigned int y, void * color)
{
	unsigned int c;
	unsigned int pixelId = ((m_width * m_components) * y) + (x * m_components);

	switch(m_dataType)
	{
	case M_UBYTE:
		for(c=0; c<m_components; c++)
			((unsigned char *)color)[c] = ((unsigned char *)m_data)[pixelId+c];
		break;
	case M_USHORT:
		for(c=0; c<m_components; c++)
			((unsigned short *)color)[c] = ((unsigned short *)m_data)[pixelId+c];
		break;
	case M_INT:
		for(c=0; c<m_components; c++)
			((int *)color)[c] = ((int *)m_data)[pixelId+c];
		break;
	case M_FLOAT:
		for(c=0; c<m_components; c++)
			((float *)color)[c] = ((float *)m_data)[pixelId+c];
		break;
	}
}
