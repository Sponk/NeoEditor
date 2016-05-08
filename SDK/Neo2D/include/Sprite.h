#ifndef NEO_SPRITE_H
#define NEO_SPRITE_H

#include <Neo2D.h>
#include <Object2D.h>

namespace Neo2D
{

/**
 * @brief Implements a 2D image.
 *
 * This class can be used to load and display 2D sprites.
 */
class NEO2D_EXPORT Sprite: public Object2D
{
	Neo::String m_imagePath;
	unsigned int m_image;
	Neo::Vector2 m_flip;

protected:
	/**
	 * @brief Loads the registered texture image into memory.
	 */
	void loadTexture();

public:
	Sprite(int x, int y, unsigned int w, unsigned int h, const char* image, const shared_ptr<Object2D>& parent);
	virtual void draw(const Neo::Vector2& offset);

	/**
	 * @brief Retrieves the "flip" of the image.
	 *
	 * The flip refers to a 2D vector which contains the angles around the X and Z axis
	 * which the sprite shall be rotated. It can be used to "flip" the image around a mirror axis.
	 * The values are stored as Euler angles.
	 *
	 * @return The flip vector.
	 */
	const Neo::Vector2& getFlip() const
	{
		return m_flip;
	}

	/**
	 * @brief Changes the flip vector.
	 * @param m_flip The new flip vector.
	 * @see getFlip
	 */
	void setFlip(const Neo::Vector2& m_flip)
	{
		Sprite::m_flip = m_flip;
	}
};

}

#endif //NEO_SPRITE_H
