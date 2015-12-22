//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
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

#include <NeoCore.h>
#include <cstdio>
#include <cstdlib>

using namespace Neo;

Image::Image(Image &img, bool reuseData) :
	m_data(img.getData()),
	m_dataType(img.getDataType()),
	m_size(img.getSize()),
	m_width(img.getWidth()),
	m_height(img.getHeight()),
	m_components(img.getComponents()),
	m_pixelSize(img.getPixelSize())
{
	if(!reuseData)
	{
		create(img.getDataType(), img.getWidth(), img.getHeight(), img.getComponents());
		memcpy(m_data, img.getData(), m_size);
	}
}

Image::Image(void):
m_data(NULL),
m_dataType(VAR_UBYTE),
m_size(0),
m_width(0),
m_height(0),
m_components(0),
m_pixelSize(0)
{}

Image::~Image(void)
{
	SAFE_FREE(m_data);
}

void Image::clear(void* color)
{
	unsigned int i;
	unsigned int c;

	switch(m_dataType)
	{
	case VAR_UBYTE:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((unsigned char *)m_data)[i+c] = ((unsigned char *)color)[c];
		}
		break;
	case VAR_USHORT:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((unsigned short *)m_data)[i+c] = ((unsigned short *)color)[c];
		}
		break;

	case VAR_INT:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((int *)m_data)[i+c] = ((int *)color)[c];
		}
		break;
	case VAR_FLOAT:
		for(i=0; i<m_size; i+=m_components)
		{
			for(c=0; c<m_components; c++)
				((float *)m_data)[i+c] = ((float *)color)[c];
		}
		break;

    default: break;
	}
}

void Image::create(VAR_TYPES dataType, unsigned int width, unsigned int height, unsigned int components)
{
	SAFE_FREE(m_data);

    m_width = width;
    m_height = height;
	m_components = components;

    m_size = m_components * m_width * m_height;

	switch(dataType)
	{
	default:
	case VAR_UBYTE:
		m_dataType = VAR_UBYTE;
		m_data = new unsigned char[m_size];
		m_pixelSize = sizeof(unsigned char);
		break;
	case VAR_USHORT:
		m_dataType = VAR_USHORT;
		m_data = new unsigned short[m_size];
		m_pixelSize = sizeof(unsigned short);
		break;
	case VAR_INT:
		m_dataType = VAR_INT;
		m_data = new int[m_size];
		m_pixelSize = sizeof(int);
		break;
	case VAR_FLOAT:
		m_dataType = VAR_FLOAT;
		m_data = new float[m_size];
		m_pixelSize = sizeof(float);
		break;
	}
}

void Image::writePixel(unsigned int x, unsigned int y, void * color)
{
	unsigned int c;
	unsigned int pixelId = ((m_width * m_components) * y) + (x * m_components);

	switch(m_dataType)
	{
	case VAR_UBYTE:
		for(c=0; c<m_components; c++)
			((unsigned char *)m_data)[pixelId+c] = ((unsigned char *)color)[c];
		break;
	case VAR_USHORT:
		for(c=0; c<m_components; c++)
			((unsigned short *)m_data)[pixelId+c] = ((unsigned short *)color)[c];
		break;
	case VAR_INT:
		for(c=0; c<m_components; c++)
			((int *)m_data)[pixelId+c] = ((int *)color)[c];
		break;
	case VAR_FLOAT:
		for(c=0; c<m_components; c++)
			((float *)m_data)[pixelId+c] = ((float *)color)[c];
		break;

    default: break;
	}
}

void Image::readPixel(unsigned int x, unsigned int y, void* color)
{
	unsigned int c;
	unsigned int pixelId = ((m_width * m_components) * y) + (x * m_components);

	switch(m_dataType)
	{
	case VAR_UBYTE:
		for(c=0; c<m_components; c++)
			((unsigned char *)color)[c] = ((unsigned char *)m_data)[pixelId+c];
		break;
	case VAR_USHORT:
		for(c=0; c<m_components; c++)
			((unsigned short *)color)[c] = ((unsigned short *)m_data)[pixelId+c];
		break;
	case VAR_INT:
		for(c=0; c<m_components; c++)
			((int *)color)[c] = ((int *)m_data)[pixelId+c];
		break;
	case VAR_FLOAT:
		for(c=0; c<m_components; c++)
			((float *)color)[c] = ((float *)m_data)[pixelId+c];
		break;

    default: break;
	}
}

void Image::scale(unsigned int width, unsigned int height)
{
	if(width == 0 || height == 0) return;

	// Save all important things needed for scaling
	Image oldImg(*this, true);

	float widthRatio = static_cast<float>(width)/oldImg.getWidth();
	float heightRatio = static_cast<float>(height)/oldImg.getHeight();

	// Set data pointer to NULL so SAFE_DELETE won't delete the
	// memory that is also used by oldImg!
	m_data = NULL;

	// Create a new scaled image and clear it with 0
	create(m_dataType, width, height, m_components);
	//memset(m_data, 0, m_size*m_pixelSize);

	// Allocate one pixel buffer for each thread
	void* pixArray = malloc(m_components * m_pixelSize * omp_get_max_threads());

#pragma omp parallel for
	for(int y = 0; y < height; y++)
	{
		// Get the pixel assigned to this thread
		void* pixel = static_cast<char*>(pixArray) + (omp_get_thread_num() * m_pixelSize);
		unsigned int nx = 0;
		unsigned int ny = 0;

		for (int x = 0; x < width; x++)
		{
			nx = y / heightRatio;
			ny = x / widthRatio;

			oldImg.readPixel(nx, ny, pixel);
			writePixel(x, y, pixel);
		}
	}

	free(pixArray);
}

void Image::scale(float s)
{
	scale(m_width*s, m_height*s);
}

void Image::rotate(int angle)
{
	if(angle % 360 == 0) return;

	Image oldImg(*this, true);

	// Allocate one pixel buffer for each thread
	void* pixArray = malloc(m_components * m_pixelSize * omp_get_max_threads());

	float radAngle = static_cast<float>(angle) * DEG_TO_RAD;
	float cangle = cos(radAngle);
	float sangle = sin(radAngle);

	// Calculate new edge points
	Vector2 topLeft(0,0);
	Vector2 topRight(round(static_cast<float>(m_width) * cangle), 0.0f);
	Vector2 bottomLeft(0, round(static_cast<float>(m_height) * cangle));
	Vector2 bottomRight(round(static_cast<float>(m_width) * cangle - static_cast<float>(m_height) * sangle),
						round(static_cast<float>(m_height) * cangle + static_cast<float>(m_width) * sangle));

	// Calculate min and max values
	unsigned int minvalX = min(0.0f, min(topRight.x, min(bottomLeft.x, bottomRight.x)));
	unsigned int minvalY = min(0.0f, min(topRight.y, min(bottomLeft.y, bottomRight.y)));

	unsigned int maxvalX = max(0.0f, max(topRight.x, max(bottomLeft.x, bottomRight.x)));
	unsigned int maxvalY = max(0.0f, max(topRight.y, max(bottomLeft.y, bottomRight.y)));

	unsigned int newWidth = maxvalX - minvalX;
	unsigned int newHeight = maxvalY - minvalY;

	m_data = NULL;
	create(m_dataType, newWidth, newHeight, m_components);

	memset(m_data, 0, m_size*m_pixelSize);

#pragma omp parallel for
	for(int y = 0; y < oldImg.getHeight(); y++)
	{
		// Get the pixel assigned to this thread
		void* pixel = static_cast<char*>(pixArray) + (omp_get_thread_num() * m_pixelSize);
		unsigned int nx = 0, ny = 0;
		for (int x = 0; x < oldImg.getWidth(); x++)
		{
			// TODO: Proper handling of negative values!
			nx = static_cast<int>(round(static_cast<float>(x) * cangle - static_cast<float>(y) * sangle)) % newWidth;
			ny = static_cast<int>(round(static_cast<float>(y) * cangle + static_cast<float>(x) * sangle)) % newHeight;

			oldImg.readPixel(x, y, pixel);
			writePixel(CLAMP(nx, 0, m_width-1), CLAMP(ny, 0, m_height-1), pixel);
		}
	}

	free(pixArray);
}
