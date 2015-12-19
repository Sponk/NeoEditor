//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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


#ifndef __IMAGE_H
#define __IMAGE_H

namespace Neo
{
/**
 * @brief Implements an image in memory with basic transform capabilities.
 *
 * This class can be used to represent textures or images in general and to
 * apply basic transformations like scale and rotation.
 */
class NEO_CORE_EXPORT Image
{
public:

	/**
	 * @brief Copy constructor.
	 *
	 * If reuseData is true, the new image object will use the same data pointer as
	 * the image that is copied from.
	 *
	 * @param img The image to copy.
	 * @param reuseData Should the pointer be reused?
	 */
	Image(Image& img, bool reuseData = false);
	Image(void);
	~Image(void);

private:

	void* m_data;
	VAR_TYPES m_dataType;

	unsigned int m_pixelSize;
	unsigned int m_components;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_size;

public:

	/**
	 * @brief Creates a new image buffer.
	 *
	 * This will free memory from earlier calls to this method and set up
	 * internal data structures to reflect the new properties.
	 * If the internal data pointer is NULL then no memory will be free'd.
	 *
	 * @param dataType The data type used to represent one component of a pixel.
	 * @param width The width of the image.
	 * @param height The height of the image.
	 * @param components The number of components per pixel.
	 */
	void create(VAR_TYPES dataType, unsigned int width, unsigned int height, unsigned int components);

	/**
	 * @brief Clears the image with a given color.
	 *
	 * \note When the content of the image is unknown the size of each pixel in bytes can be
	 * retrieved using getPixelSize.
	 *
	 * @param color The pointer to one pixel.
	 */
	void clear(void* color);

	/**
	 * @brief Reads one pixel from the image.
	 *
	 * The pixel data is stored into the variable the color parameter points to.
	 *
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param color The pixel to store the result in.
	 */
	void readPixel(unsigned int x, unsigned int y, void* color);

	/**
	 * @brief Writes on pixel to the image.
	 *
	 * Writes the pixel data in the variable the color parameter points to
	 * into the image.
	 *
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param color The pixel to write.
	 */
	void writePixel(unsigned int x, unsigned int y, void* color);

	/**
	 * @brief Scales the image to a given width and height.
	 *
	 * The algorithm does not interpolate, that means results
	 * look rather blocky when scaling up.
	 *
	 * @param width The new width.
	 * @param height The new height.
	 */
	void scale(unsigned int width, unsigned int height);

	/**
	 * @brief Scales the image by given percentage.
	 * @param The percentage to scale with.
	 * @see scale(unsigned int width, unsigned int height)
	 */
	void scale(float s);

	/**
	 * @brief Scales the image by given percentage.
	 * @param The percentage to scale with.
	 * @see scale(unsigned int width, unsigned int height)
	 */
	void rotate(int angle);

	/**
	 * @brief Retrieves the internal data pointer.
	 * @return A void pointer to the internal pixel data.
	 */
	void* getData(void){ return m_data; }

	/**
	 * @brief Retrieves the data type used for each component of a pixel.
	 * @return The data type of each pixel component.
	 */
	VAR_TYPES getDataType(void){ return m_dataType; }

	/**
	 * @return The number of components per pixel.
	 */
	unsigned int getComponents(void){ return m_components; }

	/**
	 * @return The width of the image.
	 */
	unsigned int getWidth(void){ return m_width; }

	/**
	 * @return The height of the image.
	 */
	unsigned int getHeight(void){ return m_height; }

	/**
	 * @return The total number of components in the image.
	 */
	unsigned int getSize(void){ return m_size; }

	/**
	 * @return The total size of one pixel in the image in bytes.
	 */
	unsigned int getPixelSize(void) { return m_pixelSize; }
};
}

#endif
