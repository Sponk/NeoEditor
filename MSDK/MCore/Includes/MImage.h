/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MImage.h
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


#ifndef _M_IMAGE_H
#define _M_IMAGE_H


class M_CORE_EXPORT MImage
{
public:

	MImage(void);
	~MImage(void);

private:

	void *  m_data;
	M_TYPES m_dataType;

	unsigned int m_components;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_size;

public:

	void create(M_TYPES dataType, unsigned int width, unsigned int height, unsigned int components);
	void clear(void * color);
	void readPixel(unsigned int x, unsigned int y, void * color);
	void writePixel(unsigned int x, unsigned int y, void * color);
	
	inline void * getData(void){ return m_data; }
	M_TYPES getDataType(void){ return m_dataType; }

	inline unsigned int getComponents(void){ return m_components; }
	inline unsigned int getWidth(void){ return m_width; }
	inline unsigned int getHeight(void){ return m_height; }
	inline unsigned int getSize(void){ return m_size; }
};


#endif